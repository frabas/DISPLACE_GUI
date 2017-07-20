// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <helpers.h>

#include <map>
#include <string>
#include <vector>
#include <queue>

#include <ostream>
#include <iostream>

#include <values.h>
#include <Vessel.h>
#include <Ship.h>
#include <Node.h>
#include <Benthos.h>
#include <Metier.h>

#ifndef NO_IPC
#include <outputqueuemanager.h>
#include <messages/movevesseloutputmessage.h>
#include <messages/moveshipoutputmessage.h>
#include <messages/vessellogbookoutputmessage.h>
#else
#include <messages/noipc.h>
#endif

#include <outputexporter.h>

using namespace std;

#include <thread>
#include <condition_variable>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#endif

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
extern bool use_dtrees;
extern bool use_gnuplot;
extern bool gui_move_vessels;
extern vector<int> ve;
extern vector <Vessel*> vessels;
extern vector <Ship*> ships;
extern vector <Population* > populations;
extern vector <Benthos* > benthoss;
extern int tstep;
extern int nbpops;
extern int export_vmslike;
extern double graph_res;
extern bool is_individual_vessel_quotas;
extern bool check_all_stocks_before_going_fishing;
extern vector <int> tariff_pop;
extern int freq_update_tariff_code;
extern int freq_do_growth;
extern int freq_redispatch_pop;
extern vector<double> arbitary_breaks_for_tariff;
extern int total_amount_credited;
extern double tariff_annual_hcr_percent_change;
extern bool is_tacs;
extern bool is_fishing_credits;
extern bool is_discard_ban;
extern bool is_impact_benthos_N;
extern vector <int> implicit_pops;
extern displace::commons::Scenario scenario;
extern DynAllocOptions dyn_alloc_sce;
extern PopSceOptions dyn_pop_sce;
extern string biolsce;
extern string fleetsce;
extern int create_a_path_shop;
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
extern vector<double> spe_percent_tac_per_pop;
extern vector <Node* > nodes;
extern multimap<int, string> harbour_names;
extern vector<int> name_metiers;
extern ofstream freq_cpue;
extern ofstream freq_profit;
extern ofstream freq_distance;
extern ofstream vmslike2;
extern ofstream vmslike3;
extern vector <Metier*> metiers;
extern ofstream export_individual_tacs;
extern vector<PathShop> pathshops;
extern ofstream fishfarmslogs;

extern void guiSendVesselLogbook(const std::string &line);



static void manage_ship(int idx_v)
{
    dout(cout << "idx_v is " << idx_v << " " << ships.size() << endl);
    ships.at(idx_v - 5000)->lock();

    std::unique_lock<std::mutex> locker(glob_mutex);
    dout(cout<<"before at (" << ships.at(idx_v - 5000)->get_x() << "," << ships.at(idx_v - 5000)->get_y()  << ") "   << endl);
    ships.at(idx_v - 5000)->move();
    dout(cout<<"after at (" << ships.at(idx_v - 5000)->get_x() << "," << ships.at(idx_v - 5000)->get_y()  << ") "   << endl);
    mOutQueue.enqueue(std::shared_ptr<OutputMessage>(new MoveShipOutputMessage(tstep, ships.at(idx_v - 5000))));
    locker.unlock();

    ships.at(idx_v - 5000)->unlock();
}




static void manage_vessel(int idx_v)
{
    vector <double> dist_to_ports;

    dout(cout  << "----------" << endl);

    glob_mutex.lock();
    int index_v =  ve[idx_v];
    outc(cout  <<  ve[idx_v] << " idx of the vessel " << vessels[ ve[idx_v] ]->get_name()<< " " << endl);
    glob_mutex.unlock();

    // check roadmap
    vessels[index_v]->lock();
    bool roadmap_empty = vessels[ index_v ]->get_roadmap().empty();
    int possible_metiers_size = vessels[ index_v ]->get_possible_metiers().size();
    bool inharbour = vessels[ index_v ]->get_inharbour();
    bool inactive = vessels[ index_v ]-> get_inactive();
    vessels[index_v]->unlock();

    if(roadmap_empty)
    {
        // check if the vessel is actually active this quarter
        // (when at least one possible metier within this quarter)
        if(possible_metiers_size > 1)
        {
            outc(cout  << "ROADMAP EMPTY" << endl);
            if(inharbour)
            {
                outc(cout  << "IN HARB" << endl);

                // LAND the catches when arriving in port and DECLARE IN LOGBOOK
                // i.e. write this trip down in the logbook output file
                // i.e. just arrived!
                if(!inactive)
                {
                    outc(cout  << "...just arrived!" << endl);
                    vessels[index_v]->updateTripsStatistics(populations, implicit_pops);
                    mOutQueue.enqueue(std::shared_ptr<OutputMessage>(new VesselLogbookOutputMessage(tstep, vessels[index_v], populations, implicit_pops)));
                    OutputExporter::instance().exportLogLike(tstep, vessels[index_v], populations, implicit_pops);
#if 0
                    std::ostringstream ss;
                    vessels[ index_v ]->export_loglike (ss, populations, tstep, nbpops);
                    loglike << ss.str();

                    guiSendVesselLogbook(ss.str());
#endif
                    //vessels[ index_v ]->export_loglike_prop_met (loglike_prop_met, tstep, nbpops);
                    vessels[index_v]->lock();
                    vessels[ index_v ]->reinit_after_a_trip();
                    vessels[index_v]->unlock();
                }
                // ***************make a probable decision*************************
                vessels[ index_v ]->which_metier_should_i_go_for(metiers);
                dout(cout << vessels[ index_v ]->get_name() << " should go for metier " << vessels[ index_v ]->get_metier()->get_name() << endl);

                // the metier should help informing to which fish price time series the vessel will be look at
                // if a decision on the fish price is used in the GoFishing dtree...
                // (interesting stocks for this vessel are given in Vessel::get_metier_target_stocks() )
                // ***************make a dtree decision****************************
                int go_fishing= vessels[ index_v ]->should_i_go_fishing( tstep,
                                                                         use_dtrees,
                                                                         dyn_alloc_sce,
                                                                         implicit_pops,
                                                                         is_individual_vessel_quotas,
                                                                         check_all_stocks_before_going_fishing);
                //}
                // ***************implement a decision*****************************
                bool do_nothing=0;
                if(go_fishing)
                {

                    //go fishing
                    outc(cout  << "GO FISHING" << endl);
                    do_nothing = vessels[ index_v ]->choose_a_ground_and_go_fishing(
                                tstep, scenario, use_dtrees,
                                dyn_alloc_sce, create_a_path_shop,  pathshops,
                                adjacency_map, relevant_nodes, nodes_in_polygons,
                                nodes,
                                metiers,
                                freq_cpue, freq_profit, freq_distance
                                );


                }
                if(!go_fishing || do_nothing) // if not going and forced to do nothing because every locations is closed for this vessel, then:
                {
                    //have some rest in the harbour
                    outc(cout  << "STAY IN HARBOUR" << endl);
                    // and decrease the rest time...
                    vessels[ index_v ]-> set_timeforrest( vessels[ index_v ]-> get_timeforrest() - PING_RATE );
                    vessels[ index_v ]-> set_next_xy( vessels[index_v ]->get_x(), vessels[ index_v ]->get_y() );
                    dout(cout  << "...for the next " << vessels[ index_v ]-> get_timeforrest() << " steps" << endl);

                }

            }
            else
            {

                outc(cout  << "NOT IN HARB...SO ON A FISHING GROUND!" << endl);
                // ***************make a decision************************************
                map<string,int> external_states_relevant_for_stopping_fishing;
                external_states_relevant_for_stopping_fishing.insert(make_pair(" none ",0));
                int stop_fishing = vessels[ index_v ]->should_i_stop_fishing(
                            external_states_relevant_for_stopping_fishing,
                            use_dtrees,
                            tstep,
                            dyn_alloc_sce, create_a_path_shop,
                            pathshops,
                            adjacency_map, relevant_nodes,
                            nodes,
                            metiers,
                            freq_cpue, freq_distance,
                            dist_to_ports);

                //....unless we got a message (e.g. at the end of a year-quarter)
                dout(cout  << "message: " << vessels[ index_v ]->read_message() << endl);
                bool force_another_ground=false;
                // check my mailbox: Am I forced to change of ground?...
                if(vessels[ index_v ]->read_message()==1)
                {
                    if(vessels[ index_v ]->get_fgrounds().size()<3)
                    {
                        //in this case, forced to go back to port instead!
                        stop_fishing=true;
                    }
                    //in this case, forced to change!
                    force_another_ground=true;
                    // reset my mail box
                    vessels[ index_v ]->reset_message();
                }

                // ***************implement a decision************************************
                // go on fishing...
                if(!stop_fishing)
                {
                    outc(cout  << "OK, I´LL CONTINUE FISHING!" << endl);

                    // ***************make a decision************************************
                    map<string,int> external_states_relevant_for_change_ground;
                    external_states_relevant_for_change_ground.insert(make_pair(" none ",0));
                    int another_ground = vessels[ index_v ]->should_i_change_ground(
                                external_states_relevant_for_change_ground, false);

                    // TO DO: do not change of ground if nodes are too far from each other e.g. nsea in myfish app
                    // so also need to test for current node code_area to see if vessel *allowed* to change grounds...

                    // ***************implement the decision************************************
                    // ...but not on this ground!
                    if(another_ground || force_another_ground )
                    {
                        outc(cout  << "CHANGE OF GROUND, FISHERS! "  << endl);
                        vessels[ index_v ]->choose_another_ground_and_go_fishing(
                                    tstep,
                                    dyn_alloc_sce, create_a_path_shop,
                                    pathshops,
                                    adjacency_map,relevant_nodes, nodes_in_polygons,
                                    nodes,
                                    metiers,
                                    freq_cpue, freq_distance
                                    );
                        outc(cout  << "GOOD JOB, FISHERS! "  << endl);

                    }
                    // ***************implement a decision************************************
                    else // Yes, keep go on catching on this ground...
                    {
                        outc(cout  << "hey, I am fishing on " << vessels[ index_v ]->get_loc()->get_idx_node().toIndex() << endl);
                        //#pragma omp critical(docatch)
                        {
                            dout(cout  << "please, check your mail! :" << vessels[ index_v ]->read_message() << endl);
                            vessels[ index_v ]->do_catch(export_individual_tacs, populations, nodes, benthoss, implicit_pops, tstep, graph_res,
                                                         is_tacs, is_individual_vessel_quotas, check_all_stocks_before_going_fishing,
                                                         is_discard_ban, is_fishing_credits, is_impact_benthos_N);

                            // check
                            //if(vessels[ index_v ]->get_loc()->get_idx_node().toIndex()==430)
                            //{
                            //    vector <double> N_at_szgroup= vessels[ index_v ]->get_loc()->get_Ns_pops_at_szgroup(3);
                            //
                            //    for(int sz=0; sz<N_at_szgroup.size(); sz++)
                            //    {
                            //        cout << "HERE RIGHT AFTER do_catch sz " <<  N_at_szgroup.at(sz) << endl;
                            //    }
                            //}

                        }
                        // update
                        vessels[index_v]->lock();
                        vessels[ index_v ]->set_timeatsea(vessels[ index_v ]->get_timeatsea()+ PING_RATE);
                        // note: no traveled_dist_this_trip cumulated here...might be changed.
                        vessels[ index_v ]-> set_state(1);
                        double cumfuelcons;
                        if(vessels[ index_v ]->get_metier()->get_metier_type()==1)
                        {
                            //trawling (type 1)
                            cumfuelcons = vessels[ index_v ]->get_cumfuelcons()+ vessels[ index_v ]->get_fuelcons()*PING_RATE*vessels[ index_v ]->get_mult_fuelcons_when_fishing();
                            outc(cout  << "fuel cons for trawlers (metier " << vessels[ index_v ]->get_metier()->get_name() << ")" << endl);
                        }
                        else
                        {
                            // gillnetting, seining (type 2)
                            cumfuelcons = vessels[ index_v ]->get_cumfuelcons()+ vessels[ index_v ]->get_fuelcons()*PING_RATE*vessels[ index_v ]->get_mult_fuelcons_when_inactive();
                            outc(cout  << "fuel cons for gillnetters or seiners (metier " << vessels[ index_v ]->get_metier()->get_name() << ")" << endl);
                        }
                        vessels[ index_v ]->set_cumfuelcons(cumfuelcons);

                        // add for cum. effort on this node
                        vessels[ index_v ]->get_loc()->add_to_cumftime(PING_RATE);
                        vessels[index_v]->unlock();

                        outc(cout  << "my catches so far is " << vessels[ index_v ]->get_cumcatches() << endl);
                        outc(cout  << "my consumed fuel so far is " << cumfuelcons << endl);
                        outc(cout  << "my time at sea so far is " << vessels[ index_v ]->get_timeatsea() << endl);



                    }
                }
                // ***************implement a decision************************************
                else
                {
                    outc(cout  << "RETURN TO PORT, NOW! "  << endl);
                    glob_mutex.lock();
                    vessels[ index_v ]->choose_a_port_and_then_return(
                                tstep,
                                dyn_alloc_sce,
                                create_a_path_shop,
                                pathshops,
                                adjacency_map,
                                relevant_nodes,
                                nodes,
                                metiers,
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

        outc(cout  << "roadmap is not empty... ");
        // display the road map
        //list<int>::iterator pos;
        //dout(cout  << "roadmap (in): ");
        //for(pos=lst.begin(); pos!=lst.end(); pos++)
        //{
        //    dout(cout  << *pos << " ");
        //}
        //dout(cout  << endl);

        // find.next.pt.on.the.graph()
        vessels[ index_v ]->find_next_point_on_the_graph(nodes);

        outc(cout  << "CURRENT LAST POS " << vessels[ index_v ]->get_loc()->get_idx_node().toIndex() << endl);

        //dout(cout  << "roadmap (out): ");
        //lst = vessels[ index_v ]->get_roadmap();
        //for(pos=lst.begin(); pos!=lst.end(); pos++)
        //{
        //    dout(cout  << *pos << " ");
        //}

    }
    outc(cout  << endl);

    // write this movement in the output  file (hourly data if PING=1)
    // (setprecision is 6 in c++ by default)
    // for VMS, export the first year only because the file is growing too big otherwise....
    vessels[index_v]->lock();

    if(export_vmslike && tstep<8641) {
        if( vessels[ index_v ]->get_state()!=3) {
            OutputExporter::instance().exportVmsLike(tstep, vessels[index_v]);
        }
    }

    int a_number_between_1_and_10= (rand() % 10 +1);
    int nb_displayed_moves_out_of_ten = 1;
    if (use_gui && gui_move_vessels  && (a_number_between_1_and_10<nb_displayed_moves_out_of_ten+1)) {
        mOutQueue.enqueue(std::shared_ptr<OutputMessage>(new MoveVesselOutputMessage(tstep, vessels[index_v])));
    }

    // realtime gnuplot
    if(use_gnuplot)
    {
        ::mutex.lock();
        vmslike2   << vessels[ index_v ]->get_x() << " "
                   << vessels[ index_v ]->get_y() <<  endl;
        ::mutex.unlock();
    }
    vessels[index_v]->unlock();

}



static void *thread_manage_func()
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
        //        cout << "Thr " << data->thread_idx << " work " << nextidx << endl;
        locker.unlock();

        if(nextidx<5000) // caution: assuming no more 5000 fishing vessels
        {
            manage_vessel(nextidx);
        }
        else
        {
            manage_ship(nextidx);
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

void thread_vessel_init (int n)
{
    exit_flag = false;

    numthreads = n;
    thread_data = new std::thread[numthreads];
    for (unsigned int i = 0; i < numthreads; ++i) {
        thread_data[i] = std::move(std::thread([]() {
            thread_manage_func();
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
        Sleep(1000);
#else
        usleep(1000);
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
