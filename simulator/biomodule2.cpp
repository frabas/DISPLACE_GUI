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



#include <Population.h>
#include <Node.h>
#include <Vessel.h>
#include <Benthos.h>
#include <options.h>
#include <readdata.h>
#include <helpers.h>
#include <storage/sqliteoutputstorage.h>
#include "storage/tables/poptable.h"
#include "storage/tables/funcgroupstable.h"

#ifndef NO_IPC
#include <ipc.h>
#else
#include <messages/noipc.h>
#endif

#include <profiler.h>

using namespace std;

#include <thread>
#include <condition_variable>
#include <errno.h>

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#ifdef PROFILE
extern AverageProfiler mPopExportProfile;
#endif

extern std::mutex glob_mutex;

extern bool enable_sqlite_out;
extern std::shared_ptr<SQLiteOutputStorage> outSqlite;

// todo: remove this, better use a unique_lock<> instead
static void lock()
{
    glob_mutex.lock();
}

// todo: remove this, better use a unique_lock<> instead
static void unlock()
{
    glob_mutex.unlock();
}


int applyBiologicalModule2(int tstep, const string & namesimu,
                          const string & namefolderinput, const string & namefolderoutput,	const string & pathoutput,
                          ofstream & popstats,
                          ofstream &popdyn_N,
                          ofstream &popdyn_F,
                          ofstream &popdyn_SSB,
                          ofstream &popdyn_annual_indic,
                          ofstream &popnodes_inc,
                          ofstream &popnodes_impact,
                          ofstream &popnodes_cumulcatches_per_pop,
                          ofstream &nodes_envt,
                          ofstream &popnodes_cumftime,
                          ofstream &popnodes_cumsweptarea,
                          ofstream &popnodes_cumcatches,
                          ofstream &popnodes_cumcatches_with_threshold,
                          ofstream &popnodes_cumdiscards,
                          ofstream &popnodes_cumdiscardsratio,
                          ofstream &popnodes_tariffs,
                          ofstream &export_individual_tacs,
                          ofstream &popnodes_end,
                          ofstream &benthosbiomassnodes,
                          ofstream &benthosnumbernodes,
                          int nbbenthospops,
                          bool use_gui,
                          const string & popstats_filename,
                          const string & popdyn_N_filename,
                          const string & popdyn_F_filename,
                          const string & popdyn_SSB_filename,
                          const string & popnodes_inc_filename,
                          const string & popnodes_end_filename,
                          const string & popnodes_impact_filename,
                          const string & popnodes_cumulcatches_per_pop_filename,
                          const string & nodes_envt_filename,
                          const string & popnodes_cumftime_filename,
                          const string & popnodes_cumsweptarea_filename,
                          const string & popnodes_cumcatches_filename,
                          const string & popnodes_cumcatches_with_threshold_filename,
                          const string & popnodes_cumdiscards_filename,
                          const string & popnodes_cumdiscardsratio_filename,
                          const string & popnodes_tariffs_filename,
                          const string & popnodes_benthos_biomass_filename,
                          const string & popnodes_benthos_number_filename,
                          const vector<int> & tsteps_quarters, const vector<int> & tsteps_semesters,
                          const vector<int> & tsteps_years, const vector<int> & tsteps_months,
                          const vector<int> & implicit_pops,
                          const vector<double> & calib_oth_landings,
                          bool is_tacs,
                          int export_vmslike,
                          int freq_do_growth,
                          const multimap<int, double> &init_weight_per_szgroup,
                          const vector<vector <double> >&species_interactions_mortality_proportion_matrix,
                          vector<Population* >& populations,
                          vector<Node* >&nodes,
                          vector<Vessel* >& vessels,
                          vector<Benthos* >& benthoss,
                          const PopSceOptions &dyn_pop_sce,
                          const DynAllocOptions &dyn_alloc_sce,
                          vector<vector<double> > &Ws_at_szgroup,
                          vector<vector<vector<vector<double> > > > &predKernel,
                          vector<vector<double> > &searchVolMat
                           )
{




//----------------------------------------//
//----------------------------------------//
// POP DYNAMICS --------------------------//
//----------------------------------------//
//----------------------------------------//

    // export initial POPSTATS
    if(tstep==0)
    {
        // EXPORT POPSTATS FILE
            for (unsigned int sp=0; sp<populations.size(); sp++)
            {


                outc(cout << "...pop " << sp << endl;)
                if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
                {

                 popstats << setprecision(6) << fixed;

                    dout(cout  << "write down the N...");
                    // get total N from summing up N over nodes
                    populations.at(sp)->aggregate_N();
                    // tstep / pop / tot N at szgroup
                    popstats << tstep << " " << sp << " ";
                    vector <double>tot_N_at_szgroup=populations.at(sp)->get_tot_N_at_szgroup();
                    for(unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
                    {
                        // output in thousands of individuals
                        popstats  << tot_N_at_szgroup.at(sz) / 1000 << " " ;
                    }

                    // ... / tot_F_at_age
                    vector <double>tot_F_at_age=populations.at(sp)->get_tot_F_at_age();
                    for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
                    {
                                                 // output F in CUMUL over months, caution!
                        popstats  << tot_F_at_age.at(a)  << " " ;
                    }


                    // ... / SSB_per_szgroup
                    dout(cout  << "write down the SSB...");
                    populations.at(sp)->set_SSB_at_szgroup( populations.at(sp)->compute_SSB() ); // here in kilos
                    populations.at(sp)->set_proportion_mature_fish(populations.at(sp)->compute_proportion_mature_fish() ); // here in kilos
                    vector <double> SSB_per_szgroup= populations.at(sp)->get_SSB_at_szgroup();
                    // reminder: tot_N_at_szgroup are in thousand in input file
                    //  but in absolute numbers here because have been multiplied by 1000 when importing
                    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
                    {
                       popstats  << SSB_per_szgroup.at(i)  << " " ;
                    }

                    // ... / tot_M_at_age
                    vector <double>tot_M_at_age=populations.at(sp)->get_tot_M_at_age();
                    for(unsigned int a = 0; a < tot_M_at_age.size(); a++)
                    {
                                                 // output M in CUMUL over months, caution!
                        popstats  << tot_M_at_age.at(a)  << " " ;
                    }

                 popstats << " " <<  endl;

                 // to db
                 if (enable_sqlite_out)
                         outSqlite->exportPopStat(populations.at(sp),sp,  tstep);
                }



            }

    }




dout(cout  << "BEGIN: POP MODEL TASKS----------" << endl);
if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
{

    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        outc(cout << "...pop " << sp << endl;)
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {
           cout << ".....monthly pop model for pop " << sp << endl;

            int name_pop =populations.at(sp)->get_name();
            outc(cout << "apply other land on nodes..." << endl);
            vector <double> M_at_szgroup      = populations.at(sp)->get_M_at_szgroup();
            vector <double> weight_at_szgroup = populations.at(sp)->get_weight_at_szgroup();
            vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();

            /*
            if(sp==1){
                vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                 cout << "START THIS STEP: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

                vector <double> a_tot_N_at_szgroup_minus1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                for(int sz=0; sz < a_tot_N_at_szgroup_minus1_here.size(); sz++)
                 cout << "START THIS STEP: a_tot_N_at_szgroup_minus1_here[" << sz << "] is "<< a_tot_N_at_szgroup_minus1_here[sz]  << endl;
            }
            */

            auto map_oth           = populations.at(sp)->get_oth_land();

            outc(cout << "landings so far for this pop " << sp << ", before applying oth_land " <<
                populations.at(name_pop)->get_landings_so_far() << endl);

            // will be used in case of area_closure
            double cumul_oth_land_to_be_displaced=0.0;
            int a_source_node_idx=0;

            // TO DO:
            // maybe a random shuffling on a_list_nodes, otherwise the correction on the depletion when TAC close to be exhausted will always occur on the same spot....but maybe to refined.
            for(unsigned int n=0; n<a_list_nodes.size(); n++)
            {
                dout(cout << a_list_nodes.at(n)->get_idx_node().toIndex() << " ");



                // apply "other" landings
                // (also accounting for a potential multiplier (default at 1.0))
                double oth_land_this_pop_this_node=
                  map_oth[a_list_nodes.at(n)->get_idx_node()]*
                   populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);

                // magic number for a the below scenario: add a stochastic variation
                // area-based sce
                if (dyn_pop_sce.option(Options::with_stochast_oth_land))
                {
                    // lognormal error
                    double a_rnorm = rlnorm(0,0.25);
                    dout(cout  << "a_rnorm " << a_rnorm << endl);
                    oth_land_this_pop_this_node= oth_land_this_pop_this_node*a_rnorm;
                }

                dout (cout << "pop " << sp << " tentative catch in kg from others on this node " << a_list_nodes.at(n)->get_idx_node().toIndex()
                    << ": " << oth_land_this_pop_this_node << endl);
                if(oth_land_this_pop_this_node!=0)
                {
                    // apply oth_land UNLESS this is a closed node!
                    // caution: quick and dirty solution because we dont know on which node
                    // the cumul will be re-affected....but we reassign the cumul to a node
                    // not farther than 200 km than a closed node.
                    // pseudocode: if a closed node then cumul otherwise apply oth_land + cumul if <200km otherwise apply oth_land only and continue the cumul
                    //
                    if ((dyn_alloc_sce.option(Options::area_closure) ||
                         dyn_alloc_sce.option(Options::area_monthly_closure)) && dyn_alloc_sce.option(Options::closed_to_others_as_well))
                    {

                        //if (binary_search (polygon_nodes.begin(), polygon_nodes.end(), a_list_nodes.at(n)->get_idx_node().toIndex()))
                        auto grd= a_list_nodes.at(n)->get_idx_node();
                        if (nodes.at(grd.toIndex())->evaluateAreaType()==1 )
                        {
                            // then, this is a closed node!
                            cumul_oth_land_to_be_displaced+=oth_land_this_pop_this_node;
                            dout(cout  << " this is a polygon node ! " << a_list_nodes.at(n)->get_idx_node().toIndex() << endl);
                            dout(cout  << " the cumul to be displaced for this pop is " << cumul_oth_land_to_be_displaced << endl);
                         // the relative node idx with oth_land to be displaced....
                            a_source_node_idx = n;

                        }
                        else
                        {
                            //a candidate node for the reception of cumul oth_land...
                            double dist_to_this_node = dist( a_list_nodes.at(n)->get_x(),
                                a_list_nodes.at(n)->get_y(),
                                a_list_nodes.at(a_source_node_idx)->get_x(),
                                a_list_nodes.at(a_source_node_idx)->get_y()
                                );
                         // MAGIC NUMBER 200 km
                            if(dist_to_this_node<200)
                            {
                                oth_land_this_pop_this_node+= cumul_oth_land_to_be_displaced;
                                dout(cout  <<  cumul_oth_land_to_be_displaced << " oth_land displaced on " << a_list_nodes.at(n)->get_idx_node().toIndex()  << endl);

                         // reinit
                                cumul_oth_land_to_be_displaced=0.0;
                         // sinon, passe ton chemin....
                            }

                            // NO: this is not a closed node...so apply oth_land
                            // but also adding the previous cumul if suitable!
                            // (needed to impact the availability back)
                            vector <double> totN = populations.at(name_pop)->get_tot_N_at_szgroup();

                            /*
                            if(sp==1){
                                vector <double> a_tot_N_at_szgroup_before_oth_land = populations.at(sp)->get_tot_N_at_szgroup();
                                for(int sz=0; sz < a_tot_N_at_szgroup_before_oth_land.size(); sz++)
                                 cout << "BEFORE OTH: a_tot_N_at_szgroup_before_oth_land[" << sz << "] is "<< a_tot_N_at_szgroup_before_oth_land[sz]  << endl;
                            }
                            */

                            if(dyn_alloc_sce.option(Options::TACs))
                            {
                                // prevent TAC overshoot from other_landings
                                if(((populations.at(name_pop)->get_landings_so_far()/1000) +
                                        oth_land_this_pop_this_node) > populations.at(name_pop)->get_quota()) {
                                             oth_land_this_pop_this_node=populations.at(name_pop)->get_quota() - (populations.at(name_pop)->get_landings_so_far()/1000);
                                }
                            }

                            // apply_oth_land()
                            if(oth_land_this_pop_this_node>0) a_list_nodes.at(n)->apply_oth_land(name_pop, oth_land_this_pop_this_node, weight_at_szgroup, totN);

                            // then, collect and accumulate tot_C_at_szgroup
                            vector <double> a_oth_catch_per_szgroup = a_list_nodes.at(n)->get_last_oth_catch_pops_at_szgroup(name_pop);
                            vector <double> a_oth_disc_per_szgroup = a_list_nodes.at(n)->get_last_oth_disc_pops_at_szgroup(name_pop);
                            vector <double> newTotC= populations.at(name_pop)->get_tot_C_at_szgroup();
                            vector <double> newTotD= populations.at(name_pop)->get_tot_D_at_szgroup();
                            for(unsigned int szgroup=0; szgroup < a_oth_catch_per_szgroup.size();++szgroup)
                            {
                               newTotC.at(szgroup) = newTotC.at(szgroup) + a_oth_catch_per_szgroup.at(szgroup);
                               newTotD.at(szgroup) = newTotD.at(szgroup) + a_oth_disc_per_szgroup.at(szgroup);
                            }
                            populations.at(name_pop)->set_tot_C_at_szgroup(newTotC);
                            populations.at(name_pop)->set_tot_D_at_szgroup(newTotD);


                        }

                    }
                    else
                    {
                        if(dyn_alloc_sce.option(Options::TACs))
                        {
                            // prevent TAC overshoot from other_landings
                            if(((populations.at(name_pop)->get_landings_so_far()/1000) +
                                    oth_land_this_pop_this_node) > populations.at(name_pop)->get_quota()){
                                oth_land_this_pop_this_node=populations.at(name_pop)->get_quota() - (populations.at(name_pop)->get_landings_so_far()/1000);
                            }

                        }

                        // needed to impact the availability
                        vector <double> totN = populations.at(name_pop)->get_tot_N_at_szgroup();
                        if(oth_land_this_pop_this_node>0) a_list_nodes.at(n)->apply_oth_land(name_pop, oth_land_this_pop_this_node, weight_at_szgroup, totN);
                        dout(cout  << "oth_land this pop this node, check after potential correction (when total depletion): "<<  oth_land_this_pop_this_node << endl);


                        // then, collect and accumulate tot_C_at_szgroup
                        vector <double> a_oth_catch_per_szgroup = a_list_nodes.at(n)->get_last_oth_catch_pops_at_szgroup(name_pop);
                        vector <double> a_oth_disc_per_szgroup = a_list_nodes.at(n)->get_last_oth_disc_pops_at_szgroup(name_pop);
                        vector <double> newTotC= populations.at(name_pop)->get_tot_C_at_szgroup();
                        vector <double> newTotD= populations.at(name_pop)->get_tot_D_at_szgroup();
                        for(unsigned int szgroup=0; szgroup < a_oth_catch_per_szgroup.size();++szgroup)
                        {
                           newTotC.at(szgroup) = newTotC.at(szgroup) + a_oth_catch_per_szgroup.at(szgroup);
                           newTotD.at(szgroup) = newTotD.at(szgroup) + a_oth_disc_per_szgroup.at(szgroup);
                        }
                        populations.at(name_pop)->set_tot_C_at_szgroup(newTotC);
                        populations.at(name_pop)->set_tot_D_at_szgroup(newTotD);

                    }
                }

                // update landings in pop from oth landings
                double so_far = (populations.at(name_pop)->get_landings_so_far()) +
                    oth_land_this_pop_this_node;
                populations.at(name_pop)->set_landings_so_far(so_far);






            }

            dout(cout  << "THE IMPACT FROM PRESSURE ON STOCK ABUNDANCE----------" << endl);
            // impact computed for the last month from N at the start month
            // over the removals (from catches + oth_land) during this month....
            // caution with terminology: here we named "pressure" what is actually "impact"
            // i.e. a ratio, (to do: need correction...)
            cout  << "compute the impact for pop " << name_pop << endl;
            vector <double>wsz = populations[name_pop]->get_weight_at_szgroup();
            for (unsigned int n=0; n<a_list_nodes.size(); n++)
            {
                dout(cout  << "node" << a_list_nodes.at(n)->get_idx_node().toIndex() << endl);
                vector <double> N_at_szgroup_at_month_start= a_list_nodes.at(n)->get_Ns_pops_at_szgroup_at_month_start(name_pop);
                vector <double> N_at_szgroup= a_list_nodes.at(n)->get_Ns_pops_at_szgroup(name_pop);
                vector <double> removals_per_szgroup= a_list_nodes.at(n)->get_removals_pops_at_szgroup(name_pop);
                vector <double> pressure_per_szgroup_pop= a_list_nodes.at(n)->get_pressure_pops_at_szgroup(name_pop);


                double tot_removals=0;
                double tot_B=0;
                for(unsigned int szgroup=0; szgroup<N_at_szgroup_at_month_start.size(); szgroup++)
                {
                    // compute the impact as proportion of the removals
                    // in ratio to total N available at the start of the month:
                    // 1- per szgroup
                         // the pressure is a proportion
                    pressure_per_szgroup_pop[szgroup]=removals_per_szgroup[szgroup]/N_at_szgroup_at_month_start[szgroup];
                    a_list_nodes.at(n)->set_pressure_pops_at_szgroup(  name_pop, pressure_per_szgroup_pop);

                    // cumul
                    tot_removals+=(removals_per_szgroup[szgroup]*wsz[szgroup]);
                         // cumul
                    tot_B+=(N_at_szgroup_at_month_start[szgroup]*wsz[szgroup]);
                }

                    // 2- per total
                    double impact_on_pop=0;
                    if(tot_B!=0)
                    {
                        if(tot_removals!=0)
                        {
                         // this is a proportion (of biomass)
                            impact_on_pop=tot_removals/tot_B;
                            if(tot_B<0) {
                                lock();
                                cout << "negative tot B!! for this pop " << name_pop << " " <<
                                    "on node " << a_list_nodes.at(n)->get_idx_node().toIndex() << endl;
                                unlock();
                            }
                            if(tot_removals<0) {
                                lock();
                                cout << "negative tot_removals!! for this pop " << name_pop << " " <<
                                    "on node " << a_list_nodes.at(n)->get_idx_node().toIndex() << endl;
                                unlock();
                            }
                        }
                        else
                        {
                         // =>  available biomass but no catch...
                            impact_on_pop=-1;
                        }
                    }
                    else
                    {
                         // => no available biomass then no catch...
                        impact_on_pop=0;
                    }
                    a_list_nodes.at(n)->set_impact_on_pops(name_pop, impact_on_pop);
                    dout(cout  << "impact_on_pop " << impact_on_pop << endl);



                    // update, export and clear for the next time...
                    //if(export_vmslike  && impact_on_pop!=0)
                    //{
                        // tstep == 34321 is 1st december 4th year
                        if(tstep == 34321) a_list_nodes.at(n)->export_popnodes_impact(popnodes_impact, tstep, name_pop);
                        if(tstep == 34321) a_list_nodes.at(n)->export_popnodes_cumulcatches_per_pop(popnodes_cumulcatches_per_pop, tstep, name_pop);
                    //}
                         // RE-INIT if no cumul is wished
                //a_list_nodes.at(n)->clear_removals_pops_at_szgroup(name_pop);
                //a_list_nodes.at(n)->clear_impact_on_pops();
            }

            outc(cout << "landings so far for this pop " << sp << ", after applying oth_land " <<
                populations.at(name_pop)->get_landings_so_far() << endl);

            dout(cout  << endl);

            // At the aggregated population scale,
            // first, sum up the N over node and overwrite tot_N_at_szgroup....
            populations.at(sp)->aggregate_N();


            // then, compute F_at_age from F_at_szgroup as -log(N(t)/N(t-1))
            // knowing the ALK i.e.  prop szgroup in each age
            // (and no need of mortality_at_szgroup in the equ. because applied independently after)
            // remember that at init t=0, N_minus_1 = N...
            // N_minus_1 is updated AT THE VERY END of the pop model of this time step for the next
            dout(cout  << "compute this month the cumulated F_at_age on the whole pop..." << endl);


           /*
             if(populations.at(sp)->get_name()==1){
                vector <double> a_tot_N_at_szgroup_minus_1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                for(int sz=0; sz < a_tot_N_at_szgroup_minus_1_here.size(); sz++)
                 cout <<"tstep " << tstep << " Code 121:  a_tot_N_at_szgroup_minus_1_here[" << sz << "] is "<< a_tot_N_at_szgroup_minus_1_here[sz]  << endl;

                vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                 cout <<"tstep " << tstep << " Code 121:  a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

            }
            */

            populations.at(sp)->compute_tot_N_and_F_and_W_at_age();

        }
        else
        {
           outc(cout << sp << ": implicit pop => no dynamic simulated..." << endl);
        }
    }					 // end sp



    for (unsigned int sp=0; sp<populations.size(); sp++)
        {
            if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )  // not an implicit species
            {
                vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();

                vector <double> prop_M_from_species_interactions = species_interactions_mortality_proportion_matrix.at(sp);

                // apply M ---------
                // this is simply annual M divided by 12 because monthly time step...
                //cout << "apply M on the whole pop..." << endl;
                //populations.at(sp)->apply_natural_mortality(); // pble for using it if distribute_N() is not by month! i.e. the dead fish here are not removed from the nodes...
               outc(cout << "apply M on each node of the pop..." << endl);
               vector <double>  M_at_szgroup= populations.at(sp)->get_M_at_szgroup();
                vector <int> species_on_node;
                for (unsigned int n=0; n<a_list_nodes.size(); n++)
                {
                    species_on_node = a_list_nodes.at(n)-> get_pop_names_on_node();
                    vector <double> a_prop_M(prop_M_from_species_interactions.size());
                    for (unsigned int spp=0; spp<prop_M_from_species_interactions.size(); spp++)
                    {
                       a_prop_M.at(spp)=prop_M_from_species_interactions.at(spp);
                       //cout << "prop_M_from_species_interactions  is " << prop_M_from_species_interactions.at(spp) << " for spp " << spp <<
                       //           " given sp is " << sp << endl; // we should expect to always get a value!=0 here....
                       if(std::find(species_on_node.begin(), species_on_node.end(), spp) == species_on_node.end())
                        {
                            //cout << "...but spp not found on this node " <<  a_list_nodes.at(n)->get_idx_node().toIndex() << " so no effect..." << endl;
                            a_prop_M.at(spp)=0.0; // put 0 in prop if pop not found on this node
                        }
                    }
                    // keep track of the N before applying M
                    populations.at(sp)->set_a_tot_N_at_szgroup_before_applying_M(populations.at(sp)->get_tot_N_at_szgroup());

                    //... then apply M:
                    if (dyn_pop_sce.option(Options::sizeSpectra))
                     {
                        a_list_nodes.at(n)->apply_natural_mortality_at_node_from_size_spectra_approach(sp, Ws_at_szgroup, predKernel, searchVolMat);
                     }
                     else
                     {
                         a_list_nodes.at(n)->apply_natural_mortality_at_node(sp, M_at_szgroup, a_prop_M);
                     }
                }
                // then, re-aggregate by summing up the N over node and overwrite tot_N_at_szgroup....
                populations.at(sp)->aggregate_N();

                // then, re-estimate what the applied M has been, to keep it tracked later in popstats
                populations.at(sp)->compute_tot_M_at_age();


                // apply only at the beginning of the year (this is maybe not always relevant...)
                if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
                {
                   outc(cout<< "ADD RECRUITS" << endl);
                    //populations[sp]->add_recruits_from_eggs();
                    vector <double> params = populations[sp]->get_param_sr();
                    if(params.at(0)>2000) // a smart guess on the alpha param to avoid further adding a meta-option...
                    {
                       populations[sp]->add_recruits_from_a_fixed_number();
                    }
                    else
                    {
                       populations[sp]->add_recruits_from_SR();
                    }

                   outc(cout<< "COMPUTE THE CPUE MULTIPLIER FOR THIS POP" << endl);
                    // compute the cpue_multiplier
                    // at the beginning of the year as N(y)/N(y-1)
                    double sum_N_year_minus_1 = 0;
                    double sum_N_start_current_year = 0;
                    vector <double> N_at_szgroup=populations.at(sp)->get_tot_N_at_szgroup();
                    vector <double> N_at_szgroup_year_minus_1=populations.at(sp)->get_tot_N_at_szgroup_year_minus_1();
                    for(unsigned int sz=0; sz<N_at_szgroup.size(); sz++)
                    {
                        sum_N_start_current_year+=N_at_szgroup.at(sz);
                        sum_N_year_minus_1+=N_at_szgroup_year_minus_1.at(sz);
                    }

                    //  see Harley et al 2001 Canadian Journal for the hyperstability b param (CPUE=qN_t^b)
                    double a_hyperstability_param = populations.at(sp)->get_hyperstability_param();
                    populations.at(sp)->set_cpue_multiplier(pow(sum_N_start_current_year/sum_N_year_minus_1, a_hyperstability_param));
                    // e.g. have a look at plot(seq(500,3000,500)/1000,(seq(500,3000,500)/1000)^0.7)

                    dout(cout << "the cpue_multiplier is " << populations.at(sp)->get_cpue_multiplier() << endl);

                    // to be used the next year
                    populations.at(sp)->set_tot_N_at_szgroup_year_minus_1( N_at_szgroup );

                }
             }
           }
        } // end month detection




  int do_growth=0;

  for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) ) // not an implicit species
        {

                // apply growth only by semester if want to be consistent with the timeframe of the survey data
                // apply growth consistently with the time frame chosen during the parameterisation because the matrix time specific

                // timing (update at 7 a.m.)
               switch(freq_do_growth)
                 {
                   case 0:
                       if((tstep % 24)==7) do_growth=1;
                  // daily update
                       break;
                   case 1:
                       if((tstep % 168)==7) do_growth=1;
                  // weekly update
                  break;
                   case 2:
                       if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep)) do_growth=1;
                  // monthly update
                  break;
                   case 3:
                       if(binary_search (tsteps_quarters.begin(), tsteps_quarters.end(), tstep)) do_growth=1;
                  // monthly update
                  break;
                   case 4:
                      if(binary_search (tsteps_semesters.begin(), tsteps_semesters.end(), tstep)) do_growth=1;
                  // monthly update
                  break;
                 }


               if(do_growth)
                {
                   /*
                   if(sp==1){
                       vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                       for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                        cout <<"tstep " << tstep << " BEFORE GROWTH: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

                       vector <double> a_tot_N_at_szgroup_minus1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                       for(int sz=0; sz < a_tot_N_at_szgroup_minus1_here.size(); sz++)
                        cout <<"tstep " << tstep << "BEFORE GROWTH: a_tot_N_at_szgroup_minus1_here[" << sz << "] is "<< a_tot_N_at_szgroup_minus1_here[sz]  << endl;
                   }
                   */

                   outc(cout<<"tstep " << tstep << "DO GROWTH TRANSITION: caution, the matrix is time-specific in the parameterisation" << endl);
                    populations[sp]->do_growth();

                  /*  if(sp==1){
                        vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                        for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                         cout <<"tstep " << tstep << "AFTER GROWTH: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

                        vector <double> a_tot_N_at_szgroup_minus1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                        for(int sz=0; sz < a_tot_N_at_szgroup_minus1_here.size(); sz++)
                         cout <<"tstep " << tstep << "AFTER GROWTH: a_tot_N_at_szgroup_minus1_here[" << sz << "] is "<< a_tot_N_at_szgroup_minus1_here[sz]  << endl;
                    }
                  */

               }

            }
            else
            {
               outc(cout << sp << ": implicit pop => no dynamic simulated..." << endl);
            }
        }




  // restart month detection
  if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
  {

    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {

            int name_pop =populations.at(sp)->get_name();

            // at the end, store the current N for being the N minus 1 the next time we are going to compute F (e.g. the next month)
            // (this is done like this because the over-writing of N that occurs in aggregate() make it difficult to do other ways....)
            vector <double> a_tot_N_at_szgroup=populations.at(sp)->get_tot_N_at_szgroup();
               populations.at(sp)->set_tot_N_at_szgroup_month_minus_1( a_tot_N_at_szgroup );

               /*
               if(sp==1){
                   for(int sz=0; sz < a_tot_N_at_szgroup.size(); sz++)
                       cout <<"tstep " << tstep << "STORED N HERE:  a_tot_N_at_szgroup[" << sz << "]  here  is "<< a_tot_N_at_szgroup[sz]  << endl;

                   vector <double> a_tot_N_at_szgroup_minus1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                   for(int sz=0; sz < a_tot_N_at_szgroup_minus1_here.size(); sz++)
                   cout <<"tstep " << tstep << "STORED N MINUS 1:  a_tot_N_at_szgroup_minus1_here[" << sz << "]  here  is "<< a_tot_N_at_szgroup_minus1_here[sz]  << endl;
               }
               */

            // spread out the recruits
            // apply only by semester, to be consistent with the timeframe of survey data
            if(do_growth)
            {
                // at the very end, then re-dispatch over nodes to re-dispatch the recruits over the nodes....
                populations.at(sp)->distribute_N();
            }

            /*
            if(sp==1){
            populations.at(sp)->aggregate_N();
            vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
            for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                cout <<"tstep " << tstep << "AFTER distribute_N and aggregate_N:  a_tot_N_at_szgroup_here[" << sz << "]  here  is "<< a_tot_N_at_szgroup_here[sz]  << endl;
            }
            */

            //...and compute the Ns on nodes at the start of this month!
            for (unsigned int n=0; n<nodes.size(); n++)
            {
                nodes.at(n)->set_Ns_pops_at_szgroup_at_month_start(name_pop, nodes.at(n)->get_Ns_pops_at_szgroup(sp));
            }


            //... and export for plotting.
           outc(cout << "write down the popdyn...");
                         // N at szgroup
            populations.at(sp)->export_popdyn_N (popdyn_N, tstep);
                         // ...and F at age
            populations.at(sp)->export_popdyn_F (popdyn_F, tstep);
            populations.at(sp)->set_SSB_at_szgroup( populations.at(sp)->compute_SSB() ); // here in kilos
            populations.at(sp)->set_proportion_mature_fish(populations.at(sp)->compute_proportion_mature_fish() ); // here in kilos
            populations.at(sp)->export_popdyn_SSB (popdyn_SSB, tstep);


            // EXPORT POPSTATS FILE
            if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
            {


                 // exporting popstats in txt
                 popstats << setprecision(6) << fixed;

                        dout(cout  << "write down the N...");
                        // get total N from summing up N over nodes
                        populations.at(sp)->aggregate_N();
                        // tstep / pop / tot N at szgroup
                        popstats << tstep << " " << sp << " ";
                        vector <double>tot_N_at_szgroup=populations.at(sp)->get_tot_N_at_szgroup();
                        for(unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
                        {
                            // output in thousands of individuals
                            popstats  << tot_N_at_szgroup.at(sz) / 1000 << " " ;
                        }

                        /*
                        if(sp==1){
                            for(int sz=0; sz < tot_N_at_szgroup.size(); sz++)
                            cout << "EXPORTED a_tot_N_at_szgroup[" << sz << "]  here  is "<< tot_N_at_szgroup[sz]  << endl;
                        }
                        */

                        // ... / tot_F_at_age
                        vector <double>tot_F_at_age=populations.at(sp)->get_tot_F_at_age();
                        for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
                        {
                                                     // output F in CUMUL over months, caution!
                            popstats  << tot_F_at_age.at(a)  << " " ;
                        }


                        // ... / SSB_per_szgroup
                        dout(cout  << "write down the SSB...");
                        vector <double> SSB_per_szgroup ( populations.at(sp)->get_tot_N_at_szgroup().size());
                        for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
                        {

                        // reminder: tot_N_at_szgroup are in thousand in input file
                        //  but in absolute numbers here because have been multiplied by 1000 when importing
                        SSB_per_szgroup.at(i) =  populations.at(sp)->get_weight_at_szgroup().at(i) *
                                         populations.at(sp)->get_tot_N_at_szgroup().at(i) *
                                         populations.at(sp)->get_maturity_at_szgroup().at(i);
                        cout << "szgroup is " << i  << " " << endl ;
                        cout << "tot_N_at_szgroup is " << populations.at(sp)->get_tot_N_at_szgroup().at(i)  << " " << endl ;
                        cout << "maturity_at_szgroup is " << populations.at(sp)->get_maturity_at_szgroup().at(i)  << " " << endl ;
                        cout << "weight_at_szgroup is " << populations.at(sp)->get_weight_at_szgroup().at(i)  << " kg" << endl ;

                        popstats  << SSB_per_szgroup.at(i) << " " ;
                        }

                        // ... / tot_M_at_age
                        vector <double>tot_M_at_age=populations.at(sp)->get_tot_M_at_age();
                        for(unsigned int a = 0; a < tot_M_at_age.size(); a++)
                        {
                                                     // output M in CUMUL over months, caution!
                            popstats  << tot_M_at_age.at(a)  << " " ;
                        }


                     popstats << " " <<  endl;



                     // exporting in sqlite db
                     if (enable_sqlite_out)
                         outSqlite->exportPopStat(populations.at(sp),sp,  tstep);


            }

            //----------------------------------------//
            //----------------------------------------//
            // MANAGEMENT PROCEDURE-------------------//
            //----------------------------------------//
            //----------------------------------------//

                        // apply only at the beginning of the year (this is maybe not always relevant...)
                        if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
                        {
                            int namepop = populations.at(sp)->get_name();



                            if(dyn_alloc_sce.option(Options::TACs))
                            {
                               // compute a TAC for y+1 from a short-term forecast (STF)
                               // and a long-term management plan (LTMP)

                                // if more than x% of vessels cheoked then declare this stock as choking fisheries
                                int nbchoked=0;
                                for (unsigned int v=0; v<vessels.size(); v++)
                                {
                                    nbchoked+=vessels.at(v)->get_is_choked().at(sp);
                                }
                                if(nbchoked>ceil(0.5*vessels.size())) populations.at(sp)->set_is_choking_fisheries(1);


                               double multiOnTACconstraint;
                               if(dyn_alloc_sce.option(Options::twoFoldTACconstr))
                                  {
                                  multiOnTACconstraint=2.0;
                               } else{
                                  multiOnTACconstraint=1.0; // default
                               }


                               if(dyn_alloc_sce.option(Options::LTMP)){

                                   populations.at(sp)->compute_TAC(multiOnTACconstraint, 1);
                               } else{
                                   if(dyn_alloc_sce.option(Options::FMSY)){

                                       populations.at(sp)->compute_TAC(multiOnTACconstraint, 2);
                                    } else{
                                       if(dyn_alloc_sce.option(Options::FMSYrange)){
                                           populations.at(sp)->compute_TAC(multiOnTACconstraint, 3);
                                       } else{
                                         populations.at(sp)->compute_TAC(multiOnTACconstraint, 4); // statuquo is default
                                       }
                                   }
                               }

                               outc(cout<< "initialize individual vessel TAC for this coming year" << endl);
                                   // initialise the individual quota from global_TAC*percent_in_simu*percent_this_vessel
                                   for (unsigned int vsl =0; vsl < vessels.size(); vsl ++)
                                      {
                                       vessels.at(vsl)->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, sp, 1, 0.0);
                                      }
                                   outc(cout<< "compute the multiplier for oth_land in consequence of the TAC change" << endl);
                                   // to do next time oth_land will be applied: oth_land * TACy+1 / TACy
                                   vector<double> ts_tac  = populations.at(sp)->get_tac()->get_ts_tac();
                                   double TAC_y_plus_1    = ts_tac.at(ts_tac.size()-1);
                                   double TAC_y           = ts_tac.at(ts_tac.size()-2);
                                   populations.at(sp)->set_oth_land_multiplier (TAC_y_plus_1 / TAC_y);
                                   outc(cout<< "compute the multiplier for oth_land in consequence of the TAC change...ok" << endl);
                                   if(populations.at(sp)->get_oth_land_multiplier()!=
                                     // i.e. a trick to check if nan
                                    populations.at(sp)->get_oth_land_multiplier())
                                      {
                                      // likely because the TAC is nan because the stock is collapsed....
                                       populations.at(sp)->set_oth_land_multiplier (0.0);
                                      //cout << "stop: check the c++ code for oth_land_multiplier"<< endl;
                                      // int ff;
                                      // cin >>ff;

                                }
                               } else{ // if no TAC regime
                                  populations.at(sp)->set_oth_land_multiplier (1.0);
                               }

                                // export
                                // ...export the cpue and oth_land multiplier
                                populations.at(sp)->export_popdyn_annual_indic(popdyn_annual_indic, tstep);

                                // RE-INIT....
                                populations.at(sp)->clear_tot_F_at_age();
                                populations.at(sp)->clear_tot_M_at_age();
                                //=> obviously, F restart from 0 each year...
                                cout << "Adding to landings_at_end_of_years this pop "<< sp << ": " << populations.at(sp)->get_landings_so_far() << endl;
                                populations.at(sp)->add_to_landings_at_end_of_years(populations.at(sp)->get_landings_so_far());
                                populations.at(sp)->set_landings_so_far(0);
                                populations.at(sp)->clear_tot_C_at_szgroup();
                                populations.at(sp)->clear_tot_D_at_szgroup();

                            //}
                            //else
                            //{
                             //}


                        }

               }
               else
               {
                       outc(cout << sp << ": implicit pop => no dynamic simulated..." << endl);
                       // apply only at the beginning of the year (this is maybe not always relevant...)
                       if(is_tacs && binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
                          {
                          //cout << "1- Current global tac for this pop " << sp << "is " << populations.at(sp)->get_tac()->get_current_tac() << endl;
                          double tac_y_plus_1 = populations.at(sp)->get_tac()->get_current_tac();

                         // should be equivalent to do:
                         // vector<double> ts_tac = populations.at(sp)->get_tac()->get_ts_tac();
                         //  populations.at(sp)->get_tac()->add_tac_y_plus_1(ts_tac.at(0));

                          populations.at(sp)->get_tac()->add_tac_y_plus_1(tac_y_plus_1);

                          //cout << "2- Current global tac for this pop " << sp << "is " << populations.at(sp)->get_tac()->get_current_tac() << endl;;

                          for (unsigned int vsl =0; vsl < vessels.size(); vsl ++)
                             {
                             vessels.at(vsl)->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, sp, 1, 0.0);
                             }

                          }
                    }

                }






    // apply only at the beginning of the year (this is maybe not always relevant...)
    if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
    {
        // export spatial distribution of biomass pop on nodes for mapping e.g. in GIS
        // pay attention to compare a start of a year with another start of a year...(because avai key is quarter spe)
        if (export_vmslike) {
            for (unsigned int n=0; n<nodes.size(); n++)
            {
                nodes[n]->export_popnodes(popnodes_end, init_weight_per_szgroup, tstep);
                if (enable_sqlite_out) {
                    bool r=outSqlite->getPopTable()->insert(tstep, nodes[n], init_weight_per_szgroup);
                }
            }
            if (use_gui) {
                popnodes_end.flush();
                guiSendUpdateCommand(popnodes_end_filename, tstep);
            }
        }
    }


   // vector <double> a_tot_N_at_szgroup_here = populations.at(1)->get_tot_N_at_szgroup();
   // for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
   // cout << "tstep " << tstep << " AT THE MIDDLE OF BIOL: a_tot_N_at_szgroup[" << sz << "]  here  is "<< a_tot_N_at_szgroup_here[sz]  << endl;



    // EXPORT: populations statistics - Monthly

#ifdef PROFILE
    mPopExportProfile.start();
#endif

    //...and export the cumulated effort on nodes (a cumul from t=0)
    for (unsigned int n=0; n<nodes.size(); n++)
    {
        nodes.at(n)->export_popnodes_cumftime(popnodes_cumftime, tstep);
        nodes.at(n)->export_popnodes_cumsweptarea(popnodes_cumsweptarea, tstep);
        nodes.at(n)->export_popnodes_cumcatches(popnodes_cumcatches, tstep);
        nodes.at(n)->export_popnodes_cumdiscards(popnodes_cumdiscards, tstep);
        nodes.at(n)->export_popnodes_cumdiscardsratio(popnodes_cumdiscardsratio, tstep);
        if(dyn_alloc_sce.option(Options::fishing_credits)) nodes.at(n)->export_popnodes_tariffs(popnodes_tariffs, tstep);
        if(tstep == 34321) nodes.at(n)->export_popnodes(popnodes_inc, init_weight_per_szgroup, tstep); // large size output disabled if -e at 0

     }


    // about cumcatches with a threshld:
    // to get the list of nodes making xx% of the total...
    int athreshold =70; // TO DO: put as arg...
    vector <double> allcumcatches;
    double sumallcumcatches=0.0;
    for (unsigned int n=0; n<nodes.size(); n++)
    {
       double acum= nodes.at(n)->get_cumcatches();
       allcumcatches.push_back (acum);
       sumallcumcatches+=acum;
    }

    int it=0;
    vector<size_t> sorted_ids = sort_indexes_descending(allcumcatches);
    double runningsum=0;
     double amount_at_threshold (static_cast<double>(athreshold)/100*sumallcumcatches);
     do
     {
        if(it>=sorted_ids.size()) break;
        int a_idx=static_cast<int>((sorted_ids.at(it)));
        runningsum+=allcumcatches.at(a_idx);
        nodes.at(a_idx)->export_popnodes_cumcatches_with_threshold(popnodes_cumcatches_with_threshold, tstep, athreshold);
        it+=1;
        nodes.at(a_idx)->set_cumcatches_with_threshold( nodes.at(a_idx)->get_cumcatches() );
     } while(runningsum < amount_at_threshold);




     // export in db
     if (enable_sqlite_out)
     {
        for (unsigned int n=0; n<nodes.size(); n++)
         {
            outSqlite->exportPopNodes(tstep, nodes.at(n));
            if(dyn_alloc_sce.option(Options::fishing_credits)) {
                //cout << "write tariffs in db..." << endl;
                outSqlite->exportTariffNodes(tstep, nodes.at(n));
             }
            outSqlite->getPopTable()->insert(tstep, nodes[n], init_weight_per_szgroup);
          }
     }


    if(dyn_pop_sce.option(Options::impact_benthos_N))
    {
        //...and export the benthos numbers on node
       for (unsigned int n=0; n<nodes.size(); n++)
          {
          for(unsigned int funcgroup=0;funcgroup< (unsigned int)nbbenthospops; funcgroup++)
             {
                   nodes.at(n)->export_benthos_tot_number_per_funcgroup(benthosnumbernodes, tstep, funcgroup);
                   if (outSqlite)
                       outSqlite->getFuncGroupsTable()->insert(tstep, nodes.at(n), funcgroup, 1);
             }
          }

       // a recovery of the benthos number on node
       for (unsigned int b=0; b<benthoss.size(); b++)
           {
                  benthoss.at(b)->recover_benthos_tot_number_per_funcgroup();
           }
     }
    else
    {
        //...and export the benthos biomasses on node
        for (unsigned int n=0; n<nodes.size(); n++)
        {
            for(unsigned int funcgroup=0;funcgroup< (unsigned int)nbbenthospops; funcgroup++)
            {
                       nodes.at(n)->export_benthos_tot_biomass_per_funcgroup(benthosbiomassnodes, tstep, funcgroup);
                       if (outSqlite)
                           outSqlite->getFuncGroupsTable()->insert(tstep, nodes.at(n), funcgroup, 0);
            }
        }

        // a recovery of the benthos biomasses on node
        for (unsigned int b=0; b<benthoss.size(); b++)
        {
            benthoss.at(b)->recover_benthos_tot_biomass_per_funcgroup();
        }

    }


    /* Flush and updates all statistics */
    if (use_gui) {
        popnodes_cumftime.flush();
        popnodes_cumsweptarea.flush();
        popnodes_cumcatches.flush();
        popnodes_cumdiscards.flush();
        guiSendUpdateCommand(popnodes_cumftime_filename, tstep);
        guiSendUpdateCommand(popnodes_cumsweptarea_filename, tstep);
        guiSendUpdateCommand(popnodes_cumcatches_filename, tstep);
        guiSendUpdateCommand(popnodes_cumdiscards_filename, tstep);
        guiSendUpdateCommand(popnodes_cumdiscardsratio_filename, tstep);
        guiSendUpdateCommand(popnodes_cumcatches_with_threshold_filename, tstep);
        guiSendUpdateCommand(popnodes_tariffs_filename, tstep);

        popnodes_impact.flush();
        guiSendUpdateCommand(popnodes_impact_filename, tstep);

        popnodes_cumulcatches_per_pop.flush();
        guiSendUpdateCommand(popnodes_cumulcatches_per_pop_filename, tstep);

        nodes_envt.flush();
        guiSendUpdateCommand(nodes_envt_filename, tstep);


        benthosbiomassnodes.flush();
        guiSendUpdateCommand(popnodes_benthos_biomass_filename, tstep);

        benthosnumbernodes.flush();
        guiSendUpdateCommand(popnodes_benthos_number_filename, tstep);

        if (export_vmslike) {
            popnodes_inc.flush();
            guiSendUpdateCommand(popnodes_inc_filename, tstep);
        }

        popstats.flush();
        guiSendUpdateCommand(popstats_filename, tstep);

        popdyn_F.flush();
        guiSendUpdateCommand(popdyn_F_filename, tstep);

        popdyn_SSB.flush();
        guiSendUpdateCommand(popdyn_SSB_filename, tstep);

        popdyn_N.flush();
        guiSendUpdateCommand(popdyn_N_filename, tstep);
    }
#ifdef PROFILE
    mPopExportProfile.elapsed_ms();
#endif
}
dout(cout  << "END: POP MODEL TASKS----------" << endl);

   // vector <double> a_tot_N_at_szgroup_here = populations.at(1)->get_tot_N_at_szgroup();
   // for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
   // cout << "tstep " << tstep << " AT THE END OF BIOL: a_tot_N_at_szgroup[" << sz << "]  here  is "<< a_tot_N_at_szgroup_here[sz]  << endl;

return 0;
}




