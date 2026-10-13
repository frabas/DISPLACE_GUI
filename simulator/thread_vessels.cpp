// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "helpers.h"

#include <map>
#include <string>
#include <vector>
#include <queue>

#include <ostream>
#include <iostream>

#include "comstructs.h"
#include "values.h"
#include "Vessel.h"
#include "Ship.h"
#include "Node.h"
#include "Benthos.h"
#include "Metier.h"


#ifndef NO_IPC

#include "outputqueuemanager.h"
#include "messages/movevesseloutputmessage.h"
#include "messages/moveshipoutputmessage.h"
#include "messages/vessellogbookoutputmessage.h"

#else
#include "messages/noipc.h"
#endif

#include "outputexporter.h"

using namespace std;

#include <thread>
#include <condition_variable>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "SimModel.h"

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

static unsigned int numthreads;
static std::queue<int> works;
static std::mutex mutex ;
static std::condition_variable work_cond;
static std::mutex work_mutex;
static std::condition_variable completion_cond;
static unsigned int uncompleted_works;

static bool exit_flag;
static std::thread *thread_data;

#ifndef NO_IPC
extern OutputQueueManager mOutQueue;
#else
static OutputQueueManager mOutQueue;
#endif

extern std::mutex glob_mutex;
extern bool use_gui;
extern bool gui_move_vessels;
extern int nb_displayed_moves_out_of_twenty;

extern vector<int> ve;
extern vector<Benthos *> benthoss;
extern int nbsteps;
extern int nbpops;
extern int export_vmslike;
extern int export_hugefiles;
extern vector <int> tariff_pop;
extern int freq_update_tariff_code;
extern int update_tariffs_based_on_lpue_or_dpue_code;
extern int freq_do_growth;
extern int freq_redispatch_pop;
extern vector<double> arbitary_breaks_for_tariff;
extern vector<double> met_multiplier_on_arbitary_breaks_for_tariff;
extern int total_amount_credited;
extern double tariff_annual_hcr_percent_change;
extern double tech_creeping_multiplier;
extern vector <int> nbcp_coupling_pops;
extern string biolsce;
extern string fleetsce;
extern int use_static_paths;
extern adjacency_map_t adjacency_map;
extern vector<string> vertex_names;
//extern map<vertex_t, vertex_t> previous;
extern vector<types::NodeId> relevant_nodes;
extern multimap<int, int> nodes_in_polygons;
extern multimap<int, int> possible_metiers;
extern multimap<int, double> freq_possible_metiers;
extern multimap<int, double> gshape_cpue_per_stk_on_nodes;
extern multimap<int, double> gscale_cpue_per_stk_on_nodes;
extern vector<int> spe_fgrounds;
extern vector<int> spe_fgrounds_init;
extern vector<int> spe_harbours;
extern vector<double> spe_freq_fgrounds;
extern vector<double> spe_freq_fgrounds_init;
extern vector<double> spe_freq_harbours;
extern vector<double> spe_vessel_betas_per_pop;
extern multimap<int, string> harbour_names;
extern vector<int> name_metiers;
extern ofstream freq_cpue;
extern ofstream freq_profit;
extern ofstream freq_distance;
extern ofstream vmslike2;
extern ofstream vmslike3;
extern ofstream Q_DECL_IMPORT export_individual_tacs;
extern vector<PathShop> pathshops;
extern ofstream fishfarmslogs;
extern ofstream windfarmslogs;
extern ofstream shipslogs;

extern std::mutex listVesselMutex;
extern vector<int> listVesselIdForVmsLikeFPingsOnlyToExport;
extern vector<int> listVesselIdForVmsLikeToExport;
extern vector<int> listVesselIdForLogLikeToExport;
extern vector<int> listVesselIdForTripCatchPopPerSzgroupExport;


extern void guiSendVesselLogbook(const std::string &line);



static void manage_ship(SimModel &model, int idx_v)
{
    dout(cout << "idx_v is " << idx_v << " " << model.ships().size() << "\n");
    model.ships().at(idx_v - 5000)->lock();

    std::unique_lock<std::mutex> locker(glob_mutex);
    dout(cout << "before at (" << model.ships().at(idx_v - 5000)->get_x()
              << "," << model.ships().at(idx_v - 5000)->get_y() << ") " << "\n");
    model.ships().at(idx_v - 5000)->move();
    dout(cout << "after at (" << model.ships().at(idx_v - 5000)->get_x() << ","
              << model.ships().at(idx_v - 5000)->get_y() << ") " << "\n");
    mOutQueue.enqueue(std::shared_ptr<OutputMessage>(
            new MoveShipOutputMessage(model.timestep(), model.ships().at(idx_v - 5000))));
    locker.unlock();

    model.ships().at(idx_v - 5000)->unlock();
}


// timestep - accounting (not used if dyn_alloc option old_timestep_accounting).
//
// Upstream gives a vessel one action per 1 h step: a move charges the time it
// took, but a vessel that reaches its destination mid-step waits out the rest
// of the step; deciding to stop fishing takes a whole step (the vessel neither
// fishes nor moves); and the logbook is written one step after the vessel is
// back in port. Here the step is a 1 h time budget that the vessel spends on
// as many actions as fit: reach the ground and (when a full hour is left, or
// with partial_fishing any fraction) fish, decide to stop and head home, reach
// port and declare the trip, all within the step. Each action that takes time
// is a segment (Vessel::add_segment) exported as its own vmslike row.
//
// Without partial_fishing a fishing action takes a whole hour, so the part of a
// step left after reaching a ground (< 1 h) is carried to the next step
// (Vessel time carry): that step has 1 h + the carry to spend, and so on, until
// a move uses it up. The carry is dropped when the trip ends.
//
// The decisions are the upstream ones, in the upstream order; only when they
// are taken changes. Kept as a separate function so that with the option off
// manage_vessel() runs the upstream code untouched.
static void manage_vessel_timebudget(std::shared_ptr<SimModel> model, int index_v,
    vector<int> &listVesselIdForLogLikeToExport,
    vector<int> &listVesselIdForTripCatchPopPerSzgroupExport)
{
    Vessel * vessel = model->vessels()[index_v];
    const bool partial_fishing = model->scenario().dyn_alloc_sce.option(Options::partial_fishing);
    const bool is_fishing_credits = model->scenario().dyn_alloc_sce.option(Options::fishing_credits);
    const double eps = 1e-9;
        // partial_fishing: shortest fishing action (h). A shorter rest of the step on
            // a ground is dropped (the vessel waits; no catch, no decision, no time charged),
            // so tiny fragments do not trigger the full do_catch side effects (random
            // draws, tariffs, metier bookkeeping) and no step holds more than 1 h.
    const double min_fishing_fraction = 0.05;
    vector<double> dist_to_ports;
    
    vessel->lock();
    vessel->clear_segments();
    double budget = PING_RATE + vessel->get_time_carry();
    vessel->set_time_carry(0.0);
    vessel->unlock();
    
    bool freshly_departed_from_port = false;
        // upstream fishes right away (no stop/change decision) on a ground just
            // changed to; kept on the vessel so that it holds when the step runs out
            // between reaching the ground and fishing (Vessel::get_fish_without_deciding)
    bool changed_ground = false;
    bool carry_left = false; // only a ground with less than a full fishing hour left carries the rest
    
    for (int action = 0; action < 50; ++action) {
        vessel->lock();
        bool roadmap_empty = vessel->get_roadmap().empty();
        int possible_metiers_size = vessel->get_possible_metiers().size();
        bool inharbour = vessel->get_inharbour();
        bool inactive = vessel->get_inactive();
        vessel->unlock();
        
            // ---- on the way: move for what is left of the step
            if (!roadmap_empty) {
            if (budget <= eps) break;
            bool arrived = false;
            vessel->lock();
            double used = vessel->move_within(model->nodes(), model->timestep(), is_fishing_credits, budget, arrived);
            vessel->unlock();
            budget -= used;
            if (!arrived) break;
            vessel->set_fish_without_deciding(changed_ground);
            changed_ground = false;
            continue;
            
            }
        
            if (possible_metiers_size <= 1) break; // inactive this quarter (as upstream)
        
                    // ---- in port
            if (inharbour) {
            if (!inactive) {
                                // just arrived: land the catches and declare in the logbook (as upstream, one step earlier)
                vessel->updateTripsStatistics(model->populations(), model->config().implicit_pops,
                model->timestep(), model->scenario().dyn_alloc_sce);
                mOutQueue.enqueue(std::shared_ptr<OutputMessage>(
                    new VesselLogbookOutputMessage(model->timestep(), vessel,
                        model->populations(), model->config().implicit_pops)));
                std::unique_lock<std::mutex> m(listVesselMutex);
                listVesselIdForLogLikeToExport.push_back(index_v);
                if (vessel->get_vid_is_part_of_ref_fleet())
                    listVesselIdForTripCatchPopPerSzgroupExport.push_back(index_v);
                break; // the rest of the step is spent in port
                
            }
            if (action > 0) break; // one departure decision per step
            vessel->which_metier_should_i_go_for(model->metiers(), model->nodes(), model->scenario().dyn_alloc_sce);
            int go_fishing = vessel->should_i_go_fishing(model->timestep(), model->populations(),
                model->scenario().use_dtrees,
                model->scenario().dyn_alloc_sce,
                model->config().implicit_pops,
                model->scenario().is_individual_vessel_quotas,
                model->scenario().check_all_stocks_before_going_fishing);
            bool do_nothing = 0;
            if (go_fishing) {
                vessel->set_defer_move(true);
                do_nothing = vessel->choose_a_ground_and_go_fishing(
                    *model, model->timestep(), model->scenario(), model->scenario().use_dtrees,
                    model->scenario().dyn_alloc_sce, use_static_paths, pathshops,
                    adjacency_map, relevant_nodes, nodes_in_polygons, model->nodes(), model->metiers(),
                    freq_cpue, freq_profit, freq_distance);
                vessel->set_defer_move(false);
                freshly_departed_from_port = true;
                
            }
            if (!go_fishing || do_nothing) {
                vessel->set_timeforrest(vessel->get_timeforrest() - PING_RATE);
                vessel->set_next_xy(vessel->get_x(), vessel->get_y());
                vessel->reset_message();
                break;
                
            }
             budget = PING_RATE; // the trip starts with this step
            continue;
            
            }
        
            // ---- on a fishing ground
            if (budget <= eps) break;
            if (!partial_fishing && budget < PING_RATE - eps) { carry_left = true; break; } // not a full hour left: carry it
            if (partial_fishing && budget < min_fishing_fraction - eps) break; // fragment: dropped
        
            int stop_fishing = 0;
            bool force_another_ground = false;
            int shall_I_change_to_another_ground = 0;
            if (!vessel->get_fish_without_deciding()) 
            {
            map<string, int> external_states_relevant_for_stopping_fishing;
            external_states_relevant_for_stopping_fishing.insert(make_pair(" none ", 0));
            stop_fishing = vessel->should_i_stop_fishing(
                *model, external_states_relevant_for_stopping_fishing, model->scenario().use_dtrees,
                model->timestep(), model->scenario().dyn_alloc_sce, use_static_paths, pathshops,
                adjacency_map, relevant_nodes, model->nodes(), model->metiers(),
                freq_cpue, freq_distance, dist_to_ports);
            if (vessel->read_message() == 1) {
                if (vessel->get_fgrounds().size() < 3) {
                    vessel->set_reason_to_go_back(4);
                    stop_fishing = true;
                    
                }
                 force_another_ground = true;
                vessel->reset_message();
                
            }
            
        }
         vessel->set_fish_without_deciding(false);
        
            if (stop_fishing) {
                        // head home now, within this step
            glob_mutex.lock();
            vessel->choose_a_port_and_then_return(*model, model->timestep(), model->scenario().dyn_alloc_sce,
                use_static_paths, pathshops, adjacency_map, relevant_nodes,
                model->nodes(), model->metiers(), freq_cpue, freq_distance,
                dist_to_ports);
            glob_mutex.unlock();
            if (vessel->get_roadmap().empty()) break; // no path: stuck, as upstream
            continue;
            
        }
        freshly_departed_from_port = false;
        
            if (!force_another_ground) {
            map<string, int> external_states_relevant_for_change_ground;
            external_states_relevant_for_change_ground.insert(make_pair(" none ", 0));
            shall_I_change_to_another_ground = vessel->should_i_change_ground(
                *model, external_states_relevant_for_change_ground, model->scenario().use_dtrees);
            
        }
        int is_not_possible_to_change = 0;
        if (shall_I_change_to_another_ground || force_another_ground) {
            vessel->set_defer_move(true);
            is_not_possible_to_change = vessel->choose_another_ground_and_go_fishing(
                *model, model->timestep(), model->scenario().dyn_alloc_sce, use_static_paths, pathshops,
                adjacency_map, relevant_nodes, nodes_in_polygons, model->nodes(), model->metiers(),
                freq_cpue, freq_distance);
            vessel->set_defer_move(false);
            if (is_not_possible_to_change && force_another_ground) {
                glob_mutex.lock();
                vessel->choose_a_port_and_then_return(*model, model->timestep(), model->scenario().dyn_alloc_sce,
                    use_static_paths, pathshops, adjacency_map, relevant_nodes,
                    model->nodes(), model->metiers(), freq_cpue, freq_distance,
                    dist_to_ports);
                vessel->set_reason_to_go_back(4);
                glob_mutex.unlock();
                if (vessel->get_roadmap().empty()) break;
                continue;
                
            }
            if (!vessel->get_roadmap().empty()) {
                changed_ground = true; // upstream fishes on arrival without a new decision
                continue;
                
            }
            
        }
        
         // ---- fish for a full hour, or (partial_fishing) for what is left of the step
        double fraction = partial_fishing ? std::min(budget, (double)PING_RATE) : (double)PING_RATE;
        vessel->set_fishing_fraction(fraction / PING_RATE);
        vessel->do_catch(model->scenario().dyn_alloc_sce, export_individual_tacs,
        model->timestep(), model->month(), model->quarter(),
        model->populations(), model->nodes(), benthoss,
        model->config().implicit_pops, model->config().grouped_tacs,
        model->timestep(), model->scenario().graph_res,
        model->is_tacs(), model->scenario().is_individual_vessel_quotas,
        model->scenario().check_all_stocks_before_going_fishing,
        model->is_discard_ban(), model->is_realtime_closure(), model->is_grouped_tacs(),
        tech_creeping_multiplier, model->is_fishing_credits(),
        model->is_direct_killing_on_benthos(), model->is_resuspension_effect_on_benthos(),
        model->is_benthos_in_numbers());
        vessel->set_fishing_fraction(1.0);
        
        vessel->lock();
        vessel->set_timeatsea(vessel->get_timeatsea() + fraction);
        vessel->set_hasfishedatleastonce(1);
        vessel->set_timeatseasincefirstcatch(vessel->get_timeatseasincefirstcatch() + fraction);
        vessel->set_state(1);
        double fuel_per_h_scaling_a = vessel->get_fuelcons() / pow(vessel->get_speed(), 3);
        double actual_speed = vessel->get_speed();
        double litre_fuel = fuel_per_h_scaling_a * pow(actual_speed, 3);
        double cumfuelcons;
        if (vessel->get_metier()->get_metier_type() == 1) {
            cumfuelcons = vessel->get_cumfuelcons() + litre_fuel * fraction *
                vessel->get_mult_fuelcons_when_fishing() *
                vessel->get_metier()->get_fuel_reduction_multiplier();
            
        }
        else {
            cumfuelcons = vessel->get_cumfuelcons() + litre_fuel * fraction *
                vessel->get_mult_fuelcons_when_inactive() *
                vessel->get_metier()->get_fuel_reduction_multiplier();
        }
        vessel->set_cumfuelcons(cumfuelcons);
        vessel->get_loc()->add_to_cumftime(fraction);
        vessel->add_segment(1, fraction, 0.0);
        vessel->unlock();
        budget -= fraction;
        
    }
    
        // what is left of the step on a ground (less than a full fishing hour) is
        // carried to the next step; anything else left (in port, stuck without a
        // path, inactive, action limit) is dropped, so the carry stays < 1 h
        vessel->lock();
    if (carry_left && !vessel->get_inharbour() && budget > eps)
        vessel->set_time_carry(std::min(budget, (double)PING_RATE - eps));
    vessel->unlock();
    (void)freshly_departed_from_port;
    }


static void manage_vessel_before181(std::shared_ptr<SimModel> model, int index_v,
    vector<int>& listVesselIdForLogLikeToExport,
    vector<int>& listVesselIdForTripCatchPopPerSzgroupExport)
{
    Vessel* vessel = model->vessels()[index_v];

    // check roadmap
    model->vessels()[index_v]->lock();
    bool roadmap_empty = model->vessels()[index_v]->get_roadmap().empty();
    int possible_metiers_size = model->vessels()[index_v]->get_possible_metiers().size();
    bool inharbour = model->vessels()[index_v]->get_inharbour();
    bool inactive = model->vessels()[index_v]->get_inactive();
    bool freshly_departed_from_port = 0;
    model->vessels()[index_v]->unlock();

    vector<double> dist_to_ports;


    if (roadmap_empty) {
        // check if the vessel is actually active this quarter
        // (when at least one possible metier within this quarter)
        if (possible_metiers_size > 1) {
            outc(cout << "ROADMAP EMPTY" << "\n");
            if (inharbour) {
                outc(cout << "IN HARB" << "\n");

                // LAND the catches when arriving in port and DECLARE IN LOGBOOK
                // i.e. write this trip down in the logbook output file
                // i.e. just arrived!
                if (!inactive) {
                    outc(cout << model->vessels()[index_v]->get_name() << " has just arrived on port (" <<
                        model->vessels()[index_v]->get_loc()->get_idx_node() << "), the reason is: " <<
                        model->vessels()[index_v]->get_reason_to_go_back() << "\n");

                    model->vessels()[index_v]->updateTripsStatistics(model->populations(),
                        model->config().implicit_pops,
                        model->timestep(),
                        model->scenario().dyn_alloc_sce);
                    mOutQueue.enqueue(std::shared_ptr<OutputMessage>(
                        new VesselLogbookOutputMessage(model->timestep(), model->vessels()[index_v],
                            model->populations(),
                            model->config().implicit_pops)));

                    std::unique_lock<std::mutex> m(listVesselMutex);
                    listVesselIdForLogLikeToExport.push_back(index_v);
                    //OutputExporter::instance().exportLogLike(model->timestep(), model->vessels()[index_v], populations, implicit_pops);

                    if (model->vessels()[index_v]->get_vid_is_part_of_ref_fleet()) {
                        listVesselIdForTripCatchPopPerSzgroupExport.push_back(index_v);
                        //OutputExporter::instance().exportTripCatchPopPerSzgroup(model->timestep(), model->vessels()[index_v], populations, implicit_pops);
                    }

#if 0
                    std::ostringstream ss;
                    model->vessels()[index_v]->export_loglike(ss, populations, model->timestep(), nbpops);
                    loglike << ss.str();

                    guiSendVesselLogbook(ss.str());
#endif
                    //model->vessels()[ index_v ]->export_loglike_prop_met (loglike_prop_met, model->timestep(), nbpops);

                    //model->vessels()[index_v]->lock();
                    //model->vessels()[ index_v ]->reinit_after_a_trip();
                    //model->vessels()[index_v]->unlock();

                    // the vessel do not take any further move decision here
                    // because the vessel will spend step to declare the landings...
                }
                else {
                    // ***************make a probable decision*************************
                    dout(cout << model->vessels()[index_v]->get_name() << " which_metier_should_i_go_for? "
                        << "\n");
                    model->vessels()[index_v]->which_metier_should_i_go_for(model->metiers(), model->nodes(), model->scenario().dyn_alloc_sce);
                    dout(cout << model->vessels()[index_v]->get_name() << " should go for metier "
                        << model->vessels()[index_v]->get_metier()->get_name() << "\n");

                    // the metier should help informing to which fish price time series the vessel will be look at
                    // if a decision on the fish price is used in the GoFishing dtree...
                    // (interesting stocks for this vessel are given in Vessel::get_metier_target_stocks() )

                    // ***************make a dtree decision****************************
                    int go_fishing = model->vessels()[index_v]->should_i_go_fishing(model->timestep(),
                        model->populations(),
                        model->scenario().use_dtrees,
                        model->scenario().dyn_alloc_sce,
                        model->config().implicit_pops,
                        model->scenario().is_individual_vessel_quotas,
                        model->scenario().check_all_stocks_before_going_fishing);
                    //}
                    // ***************implement a decision*****************************
                    bool do_nothing = 0;
                    if (go_fishing) {

                        //go fishing
                        outc(cout << "GO FISHING" << "\n");
                        do_nothing = model->vessels()[index_v]->choose_a_ground_and_go_fishing(
                            *model,
                            model->timestep(), model->scenario(), model->scenario().use_dtrees,
                            model->scenario().dyn_alloc_sce,
                            use_static_paths,
                            pathshops,
                            adjacency_map, relevant_nodes, nodes_in_polygons,
                            model->nodes(),
                            model->metiers(),
                            freq_cpue,
                            freq_profit,
                            freq_distance
                        );
                        freshly_departed_from_port = 1;

                    }
                    if (!go_fishing || do_nothing) // if not going and forced to do nothing because every locations is closed for this vessel, then:
                    {
                        //have some rest in the harbour
                        //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  "STAY IN HARBOUR" << "\n";
                        outc(cout << "STAY IN HARBOUR" << "\n");
                        // and decrease the rest time...
                        model->vessels()[index_v]->set_timeforrest(
                            model->vessels()[index_v]->get_timeforrest() - PING_RATE);
                        model->vessels()[index_v]->set_next_xy(model->vessels()[index_v]->get_x(),
                            model->vessels()[index_v]->get_y());
                        dout(cout << "...for the next " << model->vessels()[index_v]->get_timeforrest() << " steps"
                            << "\n");
                        model->vessels()[index_v]->reset_message(); // ignore message to change fground or return to port as the vessel is actually in harbour

                    }
                } // end else{} that is taking a decision because not arriving in harb
            }
            else
            {

                outc(cout << "NOT IN HARB...SO ON A FISHING GROUND!" << "\n");
                //if((model->vessels()[index_v]->get_name())=="POL023600922") cout  << model->vessels()[index_v]->get_name() << "NOT IN HARB...SO ON A FISHING GROUND!" << "\n";
                // ***************make a decision************************************
                map<string, int> external_states_relevant_for_stopping_fishing;
                external_states_relevant_for_stopping_fishing.insert(make_pair(" none ", 0));
                int stop_fishing = model->vessels()[index_v]->should_i_stop_fishing(
                    *model,
                    external_states_relevant_for_stopping_fishing,
                    model->scenario().use_dtrees,
                    model->timestep(),
                    model->scenario().dyn_alloc_sce, use_static_paths,
                    pathshops,
                    adjacency_map, relevant_nodes,
                    model->nodes(),
                    model->metiers(),
                    freq_cpue, freq_distance,
                    dist_to_ports);

                //....unless we got a message (e.g. at the end of a year-quarter)
                dout(cout << "message: " << model->vessels()[index_v]->read_message() << "\n");
                bool force_another_ground = false;
                // check my mailbox: Am I forced to change of ground?...
                if (model->vessels()[index_v]->read_message() == 1) {
                    if (model->vessels()[index_v]->get_fgrounds().size() < 3) {
                        //in this case, forced to go back to port instead!
                        model->vessels()[index_v]->set_reason_to_go_back(4);
                        stop_fishing = true;
                    }
                    //in this case, forced to change!
                    force_another_ground = true;
                    // reset my mail box
                    model->vessels()[index_v]->reset_message();
                }

                if (freshly_departed_from_port && stop_fishing) {
                    outc(
                        cout << "OH! SOMETHING WRONG WITH MY CHOICE! CHECK MY -STOP FISHING- TRIGGERS"
                        << "\n"); // ...maybe underestimated fuel tank capacity, or an overestimated geographical range for a vessel doing daily trips
                }

                // outc() of verbose at 2
                outc(cout << model->vessels()[index_v]->get_name() << " SHOULD I STOP? (0/1): " << stop_fishing << "\n");
                outc(cout << "given cum fuel cons is: " << model->vessels()[index_v]->get_cumfuelcons() << "\n");
                outc(cout << "given total tank capacity is: " << model->vessels()[index_v]->get_tankcapacity() << "\n");
                outc(cout << "given cumcatches is: " << model->vessels()[index_v]->get_cumcatches() << "\n");
                outc(cout << "given vessel carrying cap is: " << model->vessels()[index_v]->get_carrycapacity() << "\n");
                /*if ((model->vessels()[index_v]->get_name()) == "PRTPTVELLLSVL10121") {
                }
                */


                // ***************implement a decision************************************
                // go on fishing...
                if (!stop_fishing) {
                    freshly_departed_from_port = 0;
                    outc(cout << "OK, I´LL CONTINUE FISHING on " << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "!" << "\n");
                    //cout << "OK, I´LL CONTINUE FISHING on " << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "!" << "\n";
                    //if((model->vessels()[index_v]->get_name())=="POL023600922") cout  << model->vessels()[index_v]->get_name() <<  "OK, I´LL CONTINUE FISHING!" << "\n";

                    // ***************make a decision************************************
                    map<string, int> external_states_relevant_for_change_ground;
                    external_states_relevant_for_change_ground.insert(make_pair(" none ", 0));
                    int shall_I_change_to_another_ground = 1;
                    if (!force_another_ground) {
                        shall_I_change_to_another_ground = model->vessels()[index_v]->should_i_change_ground(
                            *model,
                            external_states_relevant_for_change_ground,
                            model->scenario().use_dtrees);
                    }
                    // note: this do not change of ground if nodes are too far from each other i.e. when code area coded "10"
                    // so current node is tested for the code_area to see if vessel is *allowed* to change grounds...

                    // ***************implement the decision************************************
                    // ...but not on this ground!
                    int is_not_possible_to_change = 0;
                    if ((shall_I_change_to_another_ground || force_another_ground)) {
                        outc(cout << "CHANGE OF GROUND, FISHERS! " << "\n");
                        //cout << "CHANGE OF GROUND, FISHERS! " << "\n";
                        //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  " CHANGE OF GROUND, FISHERS! " << "\n";
                        is_not_possible_to_change = model->vessels()[index_v]->choose_another_ground_and_go_fishing(
                            *model,
                            model->timestep(),
                            model->scenario().dyn_alloc_sce, use_static_paths,
                            pathshops,
                            adjacency_map, relevant_nodes, nodes_in_polygons,
                            model->nodes(),
                            model->metiers(),
                            freq_cpue, freq_distance
                        );
                        outc(cout << "GOOD JOB, FISHERS! WE ARE NOW ON GROUND: " << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "\n");
                        //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  " GOOD JOB, FISHERS! " << "\n";
                        if (!model->vessels()[index_v]->get_roadmap().empty()) {
                            outc(cout << "THE NEW GROUND IS A BIT FAR...WE WILL NEED MORE THAN ONE PING TIME TO REACH IT...THE ROADMAP IS NOT EMPTY YET" << "\n")
                                roadmap_empty = false;
                        }
                        else {
                            // start fishing immediately on this new ground, if it is not too far away (i.e. if roadmap is empty)
                            //TODO: TEST
                            outc(cout << "THE NEW GROUND IS CLOSE ENOUGH...WE WILL START FISHING IMMEDIATELY ON THIS NEW GROUND" << "\n");
                            roadmap_empty = true;
                        }
                    }
                    // ***************implement a decision************************************
                    if (!(is_not_possible_to_change && force_another_ground) && roadmap_empty)
                        // keep go on catching on this ground...
                    {
                        outc(cout << "hey, I am fishing on "
                            << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "\n");
                        //cout << "hey, I am fishing on "
                        //    << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "\n";
                        {
                            dout(cout << "please, check your mail! :" << model->vessels()[index_v]->read_message()
                                << "\n");

                            bool debug_compare = false; // used to debug the refactoring of do_catch on version >1.5.0
                            if (debug_compare) {
                                std::ofstream dummy;
                                bool ok = model->vessels()[index_v]->debug_compare_do_catch(model->scenario().dyn_alloc_sce,
                                    export_individual_tacs,
                                    model->timestep(),
                                    model->month(),
                                    model->quarter(),
                                    model->populations(),
                                    model->nodes(),
                                    benthoss,
                                    model->config().implicit_pops,
                                    model->config().grouped_tacs,
                                    model->timestep(), model->scenario().graph_res,
                                    model->is_tacs(),
                                    model->scenario().is_individual_vessel_quotas,
                                    model->scenario().check_all_stocks_before_going_fishing,
                                    model->is_discard_ban(),
                                    model->is_realtime_closure(),
                                    model->is_grouped_tacs(),
                                    tech_creeping_multiplier,
                                    model->is_fishing_credits(),
                                    model->is_direct_killing_on_benthos(),
                                    model->is_resuspension_effect_on_benthos(),
                                    model->is_benthos_in_numbers());
                                if (!ok) {
                                    std::cerr << "do_catch vs do_catch_v150 differ – see above for details.\n";
                                }
                            }

                            model->vessels()[index_v]->do_catch(model->scenario().dyn_alloc_sce,
                                export_individual_tacs,
                                model->timestep(),
                                model->month(),
                                model->quarter(),
                                model->populations(),
                                model->nodes(),
                                benthoss,
                                model->config().implicit_pops,
                                model->config().grouped_tacs,
                                model->timestep(), model->scenario().graph_res,
                                model->is_tacs(),
                                model->scenario().is_individual_vessel_quotas,
                                model->scenario().check_all_stocks_before_going_fishing,
                                model->is_discard_ban(),
                                model->is_realtime_closure(),
                                model->is_grouped_tacs(),
                                tech_creeping_multiplier,
                                model->is_fishing_credits(),
                                model->is_direct_killing_on_benthos(),
                                model->is_resuspension_effect_on_benthos(),
                                model->is_benthos_in_numbers());

                            // check
                            //if(model->vessels()[ index_v ]->get_loc()->get_idx_node().toIndex()==430)
                            //{
                            //    vector <double> N_at_szgroup= model->vessels()[ index_v ]->get_loc()->get_Ns_pops_at_szgroup(3);
                            //
                            //    for(int sz=0; sz<N_at_szgroup.size(); sz++)
                            //    {
                            //        cout << "HERE RIGHT AFTER do_catch sz " <<  N_at_szgroup.at(sz) << "\n";
                            //    }
                            //}





                        }
                        // update
                        if (!model) {
                            std::cerr << "[FATAL] model pointer is null!\n";
                            std::terminate();
                        }

                        auto& vessels = model->vessels();          // reference to the internal container
                        if (index_v >= vessels.size()) {
                            std::cerr << "[ERROR] index_v (" << index_v
                                << ") >= vessels.size() (" << vessels.size() << ")\n";
                            std::terminate();                     // or gracefully skip this vessel
                        }

                        auto& vesselPtr = vessels[index_v];
                        if (!vesselPtr) {
                            std::cerr << "[ERROR] vessels[" << index_v << "] is nullptr!\n";
                            std::terminate();
                        }

                        // At this point we know vesselPtr is a valid object.
                        vesselPtr->lock();   // <-- no crash any more


                        model->vessels()[index_v]->set_timeatsea(
                            model->vessels()[index_v]->get_timeatsea() + PING_RATE);
                        // cout << "after do_catch, timeatsea is now uptaded to: " << model->vessels()[index_v]->get_timeatsea() << endl;
                        model->vessels()[index_v]->set_hasfishedatleastonce(1);
                        model->vessels()[index_v]->set_timeatseasincefirstcatch(
                            model->vessels()[index_v]->get_timeatseasincefirstcatch() + PING_RATE);
                        // note: no traveled_dist_this_trip cumulated here...might be changed.
                        model->vessels()[index_v]->set_state(1);
                        double cumfuelcons;
                        double fuel_per_h_scaling_a = model->vessels()[index_v]->get_fuelcons() / pow(model->vessels()[index_v]->get_speed(), 3);
                        double actual_speed = model->vessels()[index_v]->get_speed(); // for now actusl speed is the same as the max speed. so the litre_fuel will be max cons... TODO: change it.
                        double litre_fuel = fuel_per_h_scaling_a * pow(actual_speed, 3); // cubic law
                        if (model->vessels()[index_v]->get_metier()->get_metier_type() == 1) {
                            //trawling (type 1)
                            cumfuelcons = model->vessels()[index_v]->get_cumfuelcons() +
                                litre_fuel * PING_RATE *
                                model->vessels()[index_v]->get_mult_fuelcons_when_fishing() *
                                model->vessels()[index_v]->get_metier()->get_fuel_reduction_multiplier();
                            outc(cout << "fuel cons for trawlers (metier "
                                << model->vessels()[index_v]->get_metier()->get_name() << ")" << "\n");
                        }
                        else {
                            // gillnetting, seining (type 2)
                            cumfuelcons = model->vessels()[index_v]->get_cumfuelcons() +
                                litre_fuel * PING_RATE *
                                model->vessels()[index_v]->get_mult_fuelcons_when_inactive() *
                                model->vessels()[index_v]->get_metier()->get_fuel_reduction_multiplier();
                            outc(cout << "fuel cons for gillnetters or seiners (metier "
                                << model->vessels()[index_v]->get_metier()->get_name() << ")" << "\n");
                        }
                        model->vessels()[index_v]->set_cumfuelcons(cumfuelcons);

                        // add for cum. effort on this node
                        model->vessels()[index_v]->get_loc()->add_to_cumftime(PING_RATE);
                        model->vessels()[index_v]->unlock();

                        outc(cout << "my catches so far is " << model->vessels()[index_v]->get_cumcatches() << "\n");
                        outc(cout << "my consumed fuel so far is " << cumfuelcons << "\n");
                        outc(cout << "my time at sea so far is " << model->vessels()[index_v]->get_timeatsea() << "\n");
                        outc(cout << "my time at sea since first catch so far is " << model->vessels()[index_v]->get_timeatseasincefirstcatch() << "\n");


                    }
                    if (is_not_possible_to_change && force_another_ground)
                    {
                        //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  " ...go elsewhere...  " << endl;
                        outc(cout << "IMPOSSIBLE TO STAY FISHING HERE...RETURN TO PORT, NOW! " << "\n");
                        //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  " RETURN TO PORT, NOW!   " << endl;
                        glob_mutex.lock();

                        // cancel last count that was supposed to be a fishing event...
                        //remove in V >1.6.4
                        //model->vessels()[index_v]->set_timeatsea(
                        //    model->vessels()[index_v]->get_timeatsea() - PING_RATE);

                        model->vessels()[index_v]->choose_a_port_and_then_return(
                            *model,
                            model->timestep(),
                            model->scenario().dyn_alloc_sce,
                            use_static_paths,
                            pathshops,
                            adjacency_map,
                            relevant_nodes,
                            model->nodes(),
                            model->metiers(),
                            freq_cpue,
                            freq_distance,
                            dist_to_ports
                        );
                        model->vessels()[index_v]->set_reason_to_go_back(4);

                        glob_mutex.unlock();

                    }
                }
                // ***************implement a decision************************************
                else
                {
                    outc(cout << "RETURN TO PORT, NOW! " << "\n");
                    //if((model->vessels()[index_v]->get_name())=="FIN000020014") cout  << model->vessels()[index_v]->get_name() <<  " RETURN TO PORT, NOW!   " << "\n";
                    glob_mutex.lock();
                    model->vessels()[index_v]->choose_a_port_and_then_return(
                        *model,
                        model->timestep(),
                        model->scenario().dyn_alloc_sce,
                        use_static_paths,
                        pathshops,
                        adjacency_map,
                        relevant_nodes,
                        model->nodes(),
                        model->metiers(),
                        freq_cpue,
                        freq_distance,
                        dist_to_ports
                    );

                    glob_mutex.unlock();
                }

            }

        }


    }
    else
    {


        outc(cout << "roadmap is not empty... ");
        // display the road map
         /* // show the roadmap
        list<types::NodeId> road= this->get_roadmap();
        // check path
        list<types::NodeId>::iterator road_iter = road.begin();
            dout(cout << "path: ");
            for( ; road_iter != road.end(); road_iter++)
            {
               dout(cout << *road_iter << " " );
            }
            dout(cout << "\n");
        */

        bool is_fishing_credits = false;
        if (model->scenario().dyn_alloc_sce.option(Options::fishing_credits)) is_fishing_credits = true;

        // then, call to find.next.pt.on.the.graph()
        model->vessels()[index_v]->find_next_point_on_the_graph(model->nodes(), model->timestep(), is_fishing_credits);

        outc(cout << "CURRENT LAST POS " << model->vessels()[index_v]->get_loc()->get_idx_node().toIndex() << "\n");

        //dout(cout  << "roadmap (out): ");
        //lst = model->vessels()[ index_v ]->get_roadmap();
        //for(pos=lst.begin(); pos!=lst.end(); pos++)
        //{
        //    dout(cout  << *pos << " ");
        //}

    }
}


static void manage_vessel(std::shared_ptr<SimModel> model, int idx_v,
                          vector<int> &listVesselIdForVmsLikeToExport,
                          vector<int> &listVesselIdForVmsLikeFPingsOnlyToExport,
                          vector<int> &listVesselIdForLogLikeToExport,
                          vector<int> &listVesselIdForTripCatchPopPerSzgroupExport)
{
    vector<double> dist_to_ports;

    dout(cout << "----------" << "\n");

    glob_mutex.lock();
    int index_v = ve[idx_v];
    outc(cout << ve[idx_v] << " idx of the vessel " << model->vessels()[ve[idx_v]]->get_name() << " " << "\n");
    //cout << ve[idx_v] << " idx of the CURRENTLY WORKING vessel " << model->vessels()[ve[idx_v]]->get_name() << " " << "\n";
    glob_mutex.unlock();


    //Vessel* vessel = model->vessels()[index_v];

    int is_exited = model->vessels()[index_v]->get_is_vessel_exited();

    if (!is_exited) {

        // timestep-accounting: the step as a time budget (see above)
        if (!model->scenario().dyn_alloc_sce.option(Options::old_timestep_accounting)) {
            manage_vessel_timebudget(model, index_v, listVesselIdForLogLikeToExport,
                listVesselIdForTripCatchPopPerSzgroupExport);
            //=> DEFAULT v1.8.0
        }
        else {


            manage_vessel_before181(model, index_v, listVesselIdForLogLikeToExport,
                listVesselIdForTripCatchPopPerSzgroupExport);

        }

        outc(cout << "\n");

    

        // write this movement in the output  file (hourly data if PING=1)
        // (setprecision is 6 in c++ by default)
        // for VMS, export the first year only because the file is growing too big otherwise....
        model->vessels()[index_v]->lock();

    

        if (model->vessels()[index_v]->get_state() != 3) {
            // Keep the export for the last year only to avoid too large db output:
            //bool alogic = (ceil((double)model->timestep()/(double)8761) == ceil((double)nbsteps/(double)8761));
            // DOES NOT WORK UNDER UNIX, so USE:
            bool alogic;
            if(export_vmslike == 1)  alogic = (model->timestep() <= 8762);
            if(export_vmslike == 7)  alogic = (model->timestep() >= 52610 && model->timestep() <= 61369);
            //if(export_vmslike == 10) alogic = (model->timestep() >= 78889 && model->timestep() <= 87673);
            if(export_vmslike >= 10) alogic = 1;
            //tstep start 7th year = 52610
            //tstep end 7th year = 61369

            if (export_vmslike!=0 && alogic) { //  && model->timestep()<8641) {
                std::unique_lock<std::mutex> m(listVesselMutex);
                listVesselIdForVmsLikeToExport.push_back(index_v);
                //OutputExporter::instance().exportVmsLike(model->timestep(), model->vessels()[index_v]);
            }
            if (model->vessels()[index_v]->get_state() == 1 &&
                model->vessels()[index_v]->get_vid_is_part_of_ref_fleet() &&
                alogic) { // fishing state
                std::unique_lock<std::mutex> m(listVesselMutex);
                listVesselIdForVmsLikeFPingsOnlyToExport.push_back(index_v);
                // OutputExporter::instance().exportVmsLikeFPingsOnly(model->timestep(), model->vessels()[index_v],  model->populations(), implicit_pops);
            }
        }

        // roundabout to solve the hang out or low responsivness of the gui from
        // the differential speed between the gui and the simulator, this latter being faster...TODO: we could imagine doing better at some point
        int a_number_between_1_and_twenty = (rand() % 20 + 1);
        if (use_gui && gui_move_vessels &&
            (a_number_between_1_and_twenty < nb_displayed_moves_out_of_twenty + 1)) {
            mOutQueue.enqueue(std::shared_ptr<OutputMessage>(
                    new MoveVesselOutputMessage(model->timestep(), model->vessels()[index_v])));
        }
     
        model->vessels()[index_v]->unlock();
    } // end is_exited
}


static void *thread_manage_func(std::shared_ptr<SimModel> model)
{
    while (!exit_flag) {
        std::unique_lock<std::mutex> locker(work_mutex);
        while (works.size() == 0 && !exit_flag) {
            work_cond.wait(locker);
        }
        if (exit_flag) {
            // todo: is this correct?
            locker.unlock();
            break;
        }

        int nextidx = works.front();
        works.pop();
        //        cout << "Thr " << data->thread_idx << " work " << nextidx << "\n";
        locker.unlock();

        if(nextidx<5000) // caution: assuming no more 5000 fishing vessels
        {
            manage_vessel(model, nextidx,
                          listVesselIdForVmsLikeToExport,
                          listVesselIdForVmsLikeFPingsOnlyToExport,
                          listVesselIdForLogLikeToExport,
                          listVesselIdForTripCatchPopPerSzgroupExport);
        }
        else
        {
            manage_ship(*model, nextidx);
        }

        locker.lock();
        --uncompleted_works;
        //        cout << "Thr " << data->thread_idx << " Completed, " << uncompleted_works << " rem\n";
        completion_cond.notify_all();
        locker.unlock();
    }

    return 0;
}

/* Public functions */

void thread_vessel_init(int n, std::shared_ptr<SimModel> model)
{
    exit_flag = false;

    numthreads = n;
    thread_data = new std::thread[numthreads];
    for (unsigned int i = 0; i < numthreads; ++i) {
        thread_data[i] = std::move(std::thread([model]() {
            thread_manage_func(model);
        }));

#if 0 // TODO: cannot implement this in a portable way.
        struct sched_param tparam;
        int policy;

        int ret = pthread_getschedparam(thread_data[i].thread, &policy, &tparam);
        if (ret < 0) {
            perror ("getschedparam");
            exit (1);
        }

#ifdef __unix__
        tparam.__sched_priority -=2;
        policy = SCHED_RR;
#else
        tparam.sched_priority = -2;
        policy = 0;
#endif

        pthread_setschedparam(thread_data[i].thread, policy, &tparam);
#endif
    }
}

void thread_vessel_prepare()
{
    work_mutex.lock();
    uncompleted_works = 0;
    work_mutex.unlock();
}

void thread_vessel_insert_job(int idx)
{
    std::unique_lock<std::mutex> locker(work_mutex);
    works.push(idx);
    ++uncompleted_works;
    work_cond.notify_one();
}

void thread_vessel_signal_exit()
{
    exit_flag = true;
    void *rv;
    for (int i = 0; i < numthreads; ++i) {
        //        thread_vessel_insert_job(-1);
        std::unique_lock<std::mutex> locker(work_mutex);
        work_cond.notify_all();
#ifdef _WIN32
        Sleep(10000);
#else
        usleep(10000);
#endif

    }
    for (int i = 0; i < numthreads; ++i)
        thread_data[i].join();
}

void thread_vessel_wait_completed()
{
    std::unique_lock<std::mutex> locker(work_mutex);

    while (uncompleted_works > 0) {
        completion_cond.wait(locker);
    }
}


void thread_vessel_deinit()
{
    std::unique_lock<std::mutex> locker(work_mutex);
    works.empty();
    exit_flag = true;
}
