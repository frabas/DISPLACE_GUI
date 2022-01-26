// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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
#include <Metier.h>
#include <Benthos.h>
#include <options.h>
#include <readdata.h>
#include <helpers.h>
#include <fisheriesmanagmt.h>
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


bool applyBiologicalModule2(int tstep, int a_month_i, int a_quarter_i, int a_year_i, const string & namesimu,
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
                          ofstream &popnodes_nbchoked,
                          ofstream &popnodes_tariffs,
                          ofstream &export_individual_tacs,
                          ofstream &popnodes_end,
                          ofstream &benthosbiomassnodes,
                          ofstream &benthosnumbernodes,
                          int nbbenthospops,
                          int nb_mets,
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
                          const string & popnodes_nbchoked_filename,
                          const string & popnodes_tariffs_filename,
                          const string & popnodes_benthos_biomass_filename,
                          const string & popnodes_benthos_number_filename,
                          const vector<int> & tsteps_quarters, const vector<int> & tsteps_semesters,
                          const vector<int> & tsteps_years, const vector<int> & tsteps_months,
                          const vector<int> & implicit_pops,
                          const vector<double> & calib_oth_landings,
                          vector<vector <double> >& selectivity_per_stock_ogives_for_oth_land,
                          bool is_tacs,
                          bool is_other_land_as_multiplier_on_sp,
                          bool is_oth_land_per_metier,
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
                          vector<vector<double> > &searchVolMat,
                          vector<vector<double> > &juveniles_diet_preference,
                          vector<vector<double> > &adults_diet_preference,
                          vector<int> &  mat_cats
                           )
{


int will_stop=0; // see Options::DEBUG_TAC





//----------------------------------------//
//----------------------------------------//
// POP DYNAMICS --------------------------//
//----------------------------------------//
//----------------------------------------//

//!!!!!!!!!!!!!//
//!!!!!!!!!!!!!//
//!!!TSTEP 0!!!//
//!!!!!!!!!!!!!//
//!!!!!!!!!!!!!//


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

                    // ... / W_per_szgroup
                    vector <double>W_per_szgroup=populations.at(sp)->get_weight_at_szgroup();
                    for(unsigned int sz = 0; sz < W_per_szgroup.size(); sz++)
                    {

                        popstats  << W_per_szgroup.at(sz)  << " " ;
                    }


                    // ... / SSB_per_szgroup
                    dout(cout  << "write down the SSB...");
                    populations.at(sp)->set_SSB_at_szgroup( populations.at(sp)->compute_SSB() ); // here in kilos
                    populations.at(sp)->set_proportion_mature_fish(populations.at(sp)->compute_proportion_mature_fish() ); // 0 to 1
                    vector <double> SSB_per_szgroup= populations.at(sp)->get_SSB_at_szgroup();
                    // reminder: tot_N_at_szgroup are in thousand in input file
                    //  but in absolute numbers here because have been multiplied by 1000 when importing
                    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
                    {
                       popstats  << SSB_per_szgroup.at(i)  << " " ;
                    }



                 popstats << " " <<  endl;

                 // send to db
                 if (enable_sqlite_out)
                {
                         outSqlite->exportPopStat(populations.at(sp),sp,  tstep);
                         for (unsigned int n=0; n<nodes.size(); n++)
                         {
                            outSqlite->exportPopNodes(tstep, nodes.at(n));
                         }
                }



                // normally the TAC is computed at the end of the each year.
                // but here we compute the TAC "for fun" at the start tstep=0 just for checking purpose
                if(tstep==0)
                {
                    if(dyn_alloc_sce.option(Options::TACs) && dyn_alloc_sce.option(Options::DEBUG_TAC))
                    {
                       will_stop=1; // trigger to stop the simu at the end of biomodule to avoid going further after this below computation that have potential side effects
                       if(dyn_alloc_sce.option(Options::FMSY))
                          {
                          int multiOnTACconstraint=1.0;
                          if(!computeTAC(populations, sp, tstep, multiOnTACconstraint, 2))
                              throw std::runtime_error("Error while executing: computeTAC"); // use for checking
                          }
                     }
                 }


             } // end sp implcit

            } // end sp






            // EXPORT INITIAL BENTHOS STATS
            if(dyn_pop_sce.option(Options::modelBenthosInN))
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


            }

    } // END TSTEP 0






//!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!TSTEPS START MONTHS!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!//


dout(cout  << "BEGIN: POP MODEL TASKS----------" << endl);
if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
{
    // check N
    //vector <double>tot_N_at_szgroup = populations.at(0)->get_tot_N_at_szgroup();
    

    int will_I_discard_all=0; // init
    vector<int> a_mls_cat = vessels.at(0)->get_metier()->get_mls_cat_per_pop();

    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        outc(cout << "...pop " << sp << endl;)
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {
           cout << "tstep " << tstep << endl;
           cout << ".....monthly pop model (apply_oth_land and compute F) for pop " << sp << endl;
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

          
            outc(cout << "landings so far for this pop " << sp << ", before applying oth_land " <<
                populations.at(name_pop)->get_landings_so_far() << endl);

            // will be used in case of area_closure
            double cumul_oth_land_to_be_displaced=0.0;
            int a_source_node_idx=0;

            // TODO:
            // maybe a random shuffling on a_list_nodes, 
            // otherwise the correction on the depletion when TAC close to be exhausted will always occur on the same spot
            // ....but maybe too refined, so keep for later.
            
            //-------------
            // retrieve some catch info that may be necessary depending on the Options in applying oth_land
            map<types::NodeId, double> map_oth;
            vector<map<types::NodeId, double> > vect_map_oth;
            vector <double> oth_land_this_pop_this_node(a_list_nodes.size(), 0);

            if (is_oth_land_per_metier && !is_other_land_as_multiplier_on_sp) //  per met, and with absolute values for removals on node
            {
                vect_map_oth = populations.at(sp)->get_oth_land_map_per_met();
                dout(cout << "other landings per met, and with absolute values for removals on node " << endl);
                for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                {
                    //dout(cout << a_list_nodes.at(n)->get_idx_node().toIndex() << " ");
                    for (int met = 0; met < vect_map_oth.size(); ++met)
                    {
                        map_oth = vect_map_oth.at(met);
                        oth_land_this_pop_this_node.at(n) +=
                            map_oth[a_list_nodes.at(n)->get_idx_node()] *
                            populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);
                    }

                }
            }

            double Cs = 0.0;
            if (!is_oth_land_per_metier && is_other_land_as_multiplier_on_sp)
            {
                map_oth = populations.at(sp)->get_oth_land();
                dout(cout << "other landings not per met, and with a multiplier on total catch instead of absolute values " << endl);
                for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                {
                    Cs += a_list_nodes.at(n)->get_cumcatches_per_pop_this_month().at(sp);
                }
                for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                {
                    //cout << "Cs last month this pop " << sp << " is " << Cs << endl;
                          //dout(cout << a_list_nodes.at(n)->get_idx_node().toIndex() << " ");
                    oth_land_this_pop_this_node.at(n) +=
                        map_oth[a_list_nodes.at(n)->get_idx_node()] * Cs *
                        populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);
                }
            }
        
            // per metier
            vector<double> Cs_per_met(nb_mets);
            if (is_oth_land_per_metier && is_other_land_as_multiplier_on_sp)
            {
                dout(cout << "other landings per met, and with a multiplier on total catch (per met) instead of absolute values " << endl);
                vect_map_oth = populations.at(sp)->get_oth_land_map_per_met();
                if (vect_map_oth.size() != nb_mets) cout << "check for missing input pop-metier files!" << endl;
                double a_factor = populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);
                for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                {
                    Node* a_node = a_list_nodes.at(n);
                    types::NodeId idx_n = a_node->get_idx_node();
                    for (unsigned int met = 0; met < nb_mets; met++)
                    {
                        Cs_per_met.at(met) += a_node->get_cumcatches_per_pop_per_met_this_month().at(sp).at(met);
                    }
                }
                for (unsigned int met = 0; met < nb_mets; met++)
                {
                    //cout <<"process met " << met << endl;
                    map_oth = vect_map_oth[met]; // caution: this subset is time-consuming...
                    for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                    {                       
                        oth_land_this_pop_this_node[n] += map_oth[a_list_nodes.at(n)->get_idx_node()] * Cs_per_met[met] * a_factor;
                    }
                }
            }
                       
      
          
            if (!is_oth_land_per_metier && !is_other_land_as_multiplier_on_sp)//  not per met, and with absolute values for removals on node (historical default)
            {
                dout(cout << "other landings not per met, and with absolute values for removals on node (historical default) " << endl);
                map_oth = populations.at(sp)->get_oth_land();
                for (unsigned int n = 0; n < a_list_nodes.size(); n++)
                {
                    //dout(cout << a_list_nodes.at(n)->get_idx_node().toIndex() << " ");
                    oth_land_this_pop_this_node.at(n) +=
                        map_oth[a_list_nodes.at(n)->get_idx_node()] *
                        populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);

                }
            }



            // loop over nodes of this pop and apply oth_land for this node on it
            for(unsigned int n=0; n<a_list_nodes.size(); n++)
            {
                dout(cout << a_list_nodes.at(n)->get_idx_node().toIndex() << " ");

                vector <double> a_oth_catch_per_szgroup; // actually, landings only...
                vector <double> a_oth_disc_per_szgroup;
                double tot_oth_land_this_node=0;

                
                // apply "other" landings (by default, it is removing a kilo per node for this sp)
                // (also accounting for a potential multiplier (default at 1.0))
          //      if (is_other_land_as_multiplier_on_sp) {
                    // if a proportion of sp biomass is given in the oth_land layer instead of an absolute kg on node:
                    //cout << "on node " << n << " map_oth[a_list_nodes.at(n)->get_idx_node()] is " << map_oth[a_list_nodes.at(n)->get_idx_node()] << endl;
                    
                    // other land as a proportion of last month catch by explicit vessels.
          //          map_oth[a_list_nodes.at(n)->get_idx_node()] = map_oth[a_list_nodes.at(n)->get_idx_node()] * Cs;
                    //cout << "on node " << n << " Cs_on_node is " << Cs_on_node << "and therefore " << " map_oth[a_list_nodes.at(n)->get_idx_node()] is now " << map_oth[a_list_nodes.at(n)->get_idx_node()] << endl;

                    // other land as a proportion of biomass present on node.
                    //vector<double> Ns_on_node = a_list_nodes.at(n)->get_Ns_pops_at_szgroup().at(sp);
                    //vector<double> Wts_this_sp = populations.at(sp)->get_weight_at_szgroup();
                    //double biomass_on_node = 0;
                    //for (unsigned int i = 0; i < Wts_this_sp.size(); i++)  biomass_on_node += (Ns_on_node.at(i) * Wts_this_sp.at(i));
                    //map_oth[a_list_nodes.at(n)->get_idx_node()] = map_oth[a_list_nodes.at(n)->get_idx_node()] * biomass_on_node;
                    //cout << "on node " << n << " biomass_on_node is " << biomass_on_node << "and therefore " << " map_oth[a_list_nodes.at(n)->get_idx_node()] is now " << map_oth[a_list_nodes.at(n)->get_idx_node()] << endl;
          //      }





          //      double oth_land_this_pop_this_node=
          //        map_oth[a_list_nodes.at(n)->get_idx_node()]*
          //         populations.at(name_pop)->get_oth_land_multiplier() * calib_oth_landings.at(sp);
              

               // cout << "oth_land_this_pop_this_node.at(n) is " << oth_land_this_pop_this_node.at(n) << endl;


                // caution: magic number for a the below scenario: add a stochastic variation
                // area-based sce
                if (dyn_pop_sce.option(Options::with_stochast_oth_land))
                {
                    // lognormal error
                    double a_rnorm = rlnorm(0,0.25);
                    dout(cout  << "a_rnorm " << a_rnorm << endl);
                    oth_land_this_pop_this_node.at(n)= oth_land_this_pop_this_node.at(n) *a_rnorm;
                }

                dout (cout << "pop " << sp << " tentative catch in kg from others on this node " << a_list_nodes.at(n)->get_idx_node().toIndex()
                    << ": " << oth_land_this_pop_this_node.at(n) << endl);
                if(oth_land_this_pop_this_node.at(n) !=0)
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
                            cumul_oth_land_to_be_displaced+=oth_land_this_pop_this_node.at(n);
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
                                oth_land_this_pop_this_node.at(n) += cumul_oth_land_to_be_displaced;
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

                                // prevent TAC overshoot from other_landings
                                // caution: TACs given in tons and landings in kg
                                will_I_discard_all=0;
                                if(dyn_alloc_sce.option(Options::TACs))
                                {
                                     if(((populations.at(name_pop)->get_landings_so_far()) +
                                        oth_land_this_pop_this_node.at(n)) >= populations.at(name_pop)->get_quota()*1000)
                                     {
                                            if(dyn_alloc_sce.option(Options::stopGoingFishingOnFirstChokedStock))
                                            {  // STOP!
                                                oth_land_this_pop_this_node.at(n) = 0;
                                                //(populations.at(name_pop)->get_quota() * 1000) - (populations.at(name_pop)->get_landings_so_far());
                                            }
                                            else
                                            {
                                                // CONTINUE & DISCARD ALL!
                                            will_I_discard_all=1;
                                            }
                                     }
                                 }

                            // apply_oth_land()
                                try {
                                    if(oth_land_this_pop_this_node.at(n)>0) a_list_nodes.at(n)->apply_oth_land(name_pop,
                                                                                                     a_mls_cat.at(name_pop),
                                                                                                     oth_land_this_pop_this_node.at(n),
                                                                                                     weight_at_szgroup,
                                                                                                     totN,
                                                                                                     will_I_discard_all,
                                                                                                     selectivity_per_stock_ogives_for_oth_land);
                                } catch (runtime_error &) {
                                    cout << "Fail in apply_oth_land 1" << endl;
                                    return false;
                                }


                            // then, collect and accumulate tot_C_at_szgroup
                            a_oth_catch_per_szgroup = a_list_nodes.at(n)->get_last_oth_catch_pops_at_szgroup(name_pop);
                            a_oth_disc_per_szgroup = a_list_nodes.at(n)->get_last_oth_disc_pops_at_szgroup(name_pop);
                            vector <double> newTotC= populations.at(name_pop)->get_tot_C_at_szgroup();
                            vector <double> newTotD= populations.at(name_pop)->get_tot_D_at_szgroup();
                            tot_oth_land_this_node = 0;
                            if (oth_land_this_pop_this_node.at(n) > 0)
                            {
                                for (unsigned int szgroup = 0; szgroup < a_oth_catch_per_szgroup.size(); ++szgroup)
                                {
                                    tot_oth_land_this_node += a_oth_catch_per_szgroup.at(szgroup);
                                    newTotC.at(szgroup) = newTotC.at(szgroup) + a_oth_catch_per_szgroup.at(szgroup);
                                    newTotD.at(szgroup) = newTotD.at(szgroup) + a_oth_disc_per_szgroup.at(szgroup);
                                }
                                populations.at(name_pop)->set_tot_C_at_szgroup(newTotC);
                                populations.at(name_pop)->set_tot_D_at_szgroup(newTotD);
                            }

                        }

                    } // end area_closure
                    else
                    {

                        // prevent TAC overshoot from other_landings
                        // caution: TACs given is tons while landings in kg
                        will_I_discard_all=0;
                        if(dyn_alloc_sce.option(Options::TACs))
                        {
                             if(((populations.at(name_pop)->get_landings_so_far()) +
                                oth_land_this_pop_this_node.at(n)) >= populations.at(name_pop)->get_quota()*1000)
                             {
                                    if(dyn_alloc_sce.option(Options::stopGoingFishingOnFirstChokedStock))
                                    {  // STOP!
                                        oth_land_this_pop_this_node.at(n) = 0;
                                    }
                                    else
                                    {
                                        // CONTINUE & DISCARD ALL!
                                    will_I_discard_all=1; // all catches in apply_oth_land() will actually go to discards_per_szgroup, for the tracking...
                                    }
                             }
                         }

                        // needed to impact the availability
                        vector <double> totN = populations.at(name_pop)->get_tot_N_at_szgroup();
                        // apply_oth_land()
                            try {
                                if(oth_land_this_pop_this_node.at(n) >0) a_list_nodes.at(n)->apply_oth_land(name_pop,
                                                                                                 a_mls_cat.at(name_pop),
                                                                                                 oth_land_this_pop_this_node.at(n),
                                                                                                 weight_at_szgroup,
                                                                                                 totN,
                                                                                                 will_I_discard_all,
                                                                                                 selectivity_per_stock_ogives_for_oth_land);
                            } catch (runtime_error &) {
                                cout << "Fail in apply_oth_land 2" << endl;
                                return false;
                            }
                        dout(cout  << "oth_land this pop this node, check after potential correction (when total depletion): "<<  oth_land_this_pop_this_node.at(n) << endl);


                        // then, collect and accumulate tot_C_at_szgroup
                        a_oth_catch_per_szgroup = a_list_nodes.at(n)->get_last_oth_catch_pops_at_szgroup(name_pop);
                        a_oth_disc_per_szgroup = a_list_nodes.at(n)->get_last_oth_disc_pops_at_szgroup(name_pop);
                        vector <double> newTotC= populations.at(name_pop)->get_tot_C_at_szgroup();
                        vector <double> newTotD= populations.at(name_pop)->get_tot_D_at_szgroup();
                        tot_oth_land_this_node = 0;
                        if (oth_land_this_pop_this_node.at(n) > 0) 
                        {
                            for (unsigned int szgroup = 0; szgroup < a_oth_catch_per_szgroup.size(); ++szgroup)
                            {
                                tot_oth_land_this_node += a_oth_catch_per_szgroup.at(szgroup);
                                newTotC.at(szgroup) = newTotC.at(szgroup) + a_oth_catch_per_szgroup.at(szgroup);
                                newTotD.at(szgroup) = newTotD.at(szgroup) + a_oth_disc_per_szgroup.at(szgroup);
                            }
                            populations.at(name_pop)->set_tot_C_at_szgroup(newTotC);
                            populations.at(name_pop)->set_tot_D_at_szgroup(newTotD);
                        }
                    }
                }

             
                // update landings in pop from oth landings
                if(will_I_discard_all==0)
                {
                    double so_far = (populations.at(name_pop)->get_landings_so_far()) + tot_oth_land_this_node;
                                      ////oth_land_this_pop_this_node.at(n);
                   populations.at(name_pop)->set_landings_so_far(so_far);
                }

               

            }

            dout(cout  << "THE IMPACT FROM PRESSURE ON STOCK ABUNDANCE----------" << endl);
            // impact computed for the last month from N at the start month
            // over the removals (from catches + oth_land) during this month....
            // caution with terminology: here we named "pressure" what is actually "impact"
            // i.e. a ratio, (to do: need correction...)
            //cout  << "compute the impact for pop " << name_pop << endl;
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
                    a_list_nodes.at(n)->set_impact_per_pop(name_pop, impact_on_pop);
                    dout(cout  << "impact_on_pop " << impact_on_pop << endl);



                    // update, export and clear for the next time...
                    //if(export_vmslike  && impact_on_pop!=0)
                    //{
                        // initially it was tstep == 34321 is 1st december 4th year. Now it is year 5
                        if((a_year_i == 3 || a_year_i == 8) && a_month_i==12) a_list_nodes.at(n)->export_popnodes_impact(popnodes_impact, tstep, name_pop);
                        if((a_year_i == 3 || a_year_i == 8) && a_month_i==12) a_list_nodes.at(n)->export_popnodes_cumulcatches_per_pop(popnodes_cumulcatches_per_pop, tstep, name_pop);
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
            try {
                populations.at(sp)->aggregate_N();
            } catch (runtime_error &) {
                cout << "Fail in aggregate_N after impact computation" << endl;
                return false;
            }
            vector <double>tot_N_at_szgroup = populations.at(name_pop)->get_tot_N_at_szgroup();
            

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

            populations.at(sp)->compute_tot_N_and_F_and_W_at_age(a_month_i);

            cout << ".....monthly pop model for pop " << sp << "...ok" << endl;

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
                cout << ".....monthly pop model (apply M on nodes) for pop " << sp << endl;
                vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();


                // apply M ---------
                // this is simply annual M divided by 12 because monthly time step...
                //cout << "apply M on the whole pop..." << endl;
                //populations.at(sp)->apply_natural_mortality(); // pble for using it if distribute_N() is not by month! i.e. the dead fish here are not removed from the nodes...
               outc(cout << "apply M on each node of the pop..." << endl);
               vector <double>  M_at_szgroup= populations.at(sp)->get_M_at_szgroup();
               vector <int> species_on_node;
               
               double multiplier_on_M_background=1.0;
               if (a_year_i == 0 && dyn_pop_sce.option(Options::massiveMortalityEvent20perY1))
               {
                   multiplier_on_M_background = 1.2; //20 percent more in Year 1
               }
               if (a_year_i==4 && dyn_pop_sce.option(Options::massiveMortalityEvent20perY5))
               {
                   multiplier_on_M_background = 1.2; //20 percent more in Year 5
               }

               for (unsigned int n=0; n<a_list_nodes.size(); n++)
                {
                    // keep track of the N before applying M
                    populations.at(sp)->set_a_tot_N_at_szgroup_before_applying_M(populations.at(sp)->get_tot_N_at_szgroup());

                    //... then apply M:
                    if (dyn_pop_sce.option(Options::sizeSpectra))
                     {
                        vector <double> M_background_this_pop = populations.at(sp)->get_background_mortality_at_szgroup();
                      try {
                          double a_scaling = 1e4; //default
                          if (namefolderinput=="NorthSea") a_scaling = 1e2; 
                          a_list_nodes.at(n)->apply_natural_mortality_at_node_from_size_spectra_approach(sp,
                                                                                                           tstep,
                                                                                                           Ws_at_szgroup,
                                                                                                           predKernel,
                                                                                                           searchVolMat,
                                                                                                           juveniles_diet_preference,
                                                                                                           adults_diet_preference,
                                                                                                           mat_cats,
                                                                                                           M_background_this_pop,
                                                                                                           multiplier_on_M_background,
                                                                                                           a_scaling);
                      } catch (runtime_error &) {
                            cout << "Fail in apply_natural_mortality_at_node_from_size_spectra_approach" << endl;
                            return false;
                        }
                     }
                     else
                     {
                        //vector <double> prop_M_from_species_interactions = species_interactions_mortality_proportion_matrix.at(sp);
                        vector <double> prop_M_from_species_interactions(populations.size(), 0);
                        prop_M_from_species_interactions.at(sp) = 1; // 1 on diag

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
                        try {
                              a_list_nodes.at(n)->apply_natural_mortality_at_node(sp, M_at_szgroup, a_prop_M, multiplier_on_M_background);
                          } catch (runtime_error &) {
                              cout << "Fail in apply_natural_mortality_at_node" << endl;
                              return false;
                          }
                     }
                } // end over n

                // then, re-aggregate by summing up the N over node and overwrite tot_N_at_szgroup....
                populations.at(sp)->aggregate_N();

                // then, re-estimate what the applied M has been, to keep it tracked later in popstats
                populations.at(sp)->compute_tot_M_at_age();


                // apply only at the beginning of the year (this is maybe not always relevant...)
                if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
                {
                   outc(cout<< "ADD RECRUITS" << endl);
                    //populations[sp]->add_recruits_from_eggs();
                    //vector <double> params = populations[sp]->get_param_sr();
                    //if(params.at(0)>2000) // a smart guess on the alpha param to avoid further adding a meta-option...
                    //{
                    //   populations[sp]->add_recruits_from_a_fixed_number();
                    //}
                    //else
                    //{
                     //  populations[sp]->add_recruits_from_SR();
                    //}

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
                    if(dyn_pop_sce.option(Options::noHyperstability)) a_hyperstability_param=1.0;
                    populations.at(sp)->set_cpue_multiplier(pow(sum_N_start_current_year/sum_N_year_minus_1, a_hyperstability_param));
                    // e.g. have a look at plot(seq(500,3000,500)/1000,(seq(500,3000,500)/1000)^0.7)

                    dout(cout << "the cpue_multiplier is " << populations.at(sp)->get_cpue_multiplier() << endl);

                    // to be used the next year
                    populations.at(sp)->set_tot_N_at_szgroup_year_minus_1( N_at_szgroup );

                }
                cout << ".....monthly pop model (apply M on nodes) for pop " << sp << "...ok" << endl;

             } // end !=implcit
           } // end sp
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
                   cout << ".....monthly pop model (apply growth) for pop " << sp  << endl;
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

                   int is_stochastic = 0;
                   if (dyn_pop_sce.option(Options::stochasticGrowth))
                   {
                       is_stochastic = 1;
                   }
                   outc(cout<<"tstep " << tstep << "DO GROWTH TRANSITION: caution, the matrix is time-specific in the parameterisation" << endl);
                   try {
                       populations[sp]->do_growth(is_stochastic);
                     } catch (runtime_error &) {
                         cout << "Fail in do_growth" << endl;
                         return false;
                     }

                  /*  if(sp==1){
                        vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                        for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                         cout <<"tstep " << tstep << "AFTER GROWTH: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

                        vector <double> a_tot_N_at_szgroup_minus1_here = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
                        for(int sz=0; sz < a_tot_N_at_szgroup_minus1_here.size(); sz++)
                         cout <<"tstep " << tstep << "AFTER GROWTH: a_tot_N_at_szgroup_minus1_here[" << sz << "] is "<< a_tot_N_at_szgroup_minus1_here[sz]  << endl;
                    }
                  */
                   cout << ".....monthly pop model (apply growth) for pop " << sp << "...ok" << endl;

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
            cout << ".....monthly pop model (possibly add recruits, dispatch over nodes, apply MP) for pop " << sp  << endl;

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

               // apply only at the beginning of the year (this is maybe not always relevant...)
               if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
               {
                  outc(cout<< "ADD RECRUITS" << endl);
                   //populations[sp]->add_recruits_from_eggs();
                   vector <double> params = populations[sp]->get_param_sr();
                   if(params.at(0)>2000) // a smart guess on the alpha param to avoid further adding a meta-option...
                   {
                       try {
                           populations[sp]->add_recruits_from_a_fixed_number();
                         } catch (runtime_error &) {
                             cout << "Fail in add_recruits_from_a_fixed_number" << endl;
                             return false;
                         }
                   }
                   else
                   {
                       try {
                           populations[sp]->add_recruits_from_SR();
                         } catch (runtime_error &) {
                             cout << "Fail in add_recruits_from_SR" << endl;
                             return false;
                         }
                   }
                }

            // spread out the recruits
            // apply only by semester, to be consistent with the timeframe of survey data
            if(do_growth)
            {
                // at the very end, then re-dispatch over nodes to re-dispatch the recruits over the nodes....
                try {
                    populations.at(sp)->distribute_N();
                  } catch (runtime_error &) {
                      cout << "Fail in distribute_N" << endl;
                      return false;
                  }
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
             cout << "export popstats..." << endl;

                 // exporting popstats in txt
                 popstats << setprecision(6) << fixed;

                        dout(cout  << "write down the N...");
                        // get total N from summing up N over nodes
                        try {
                            populations.at(sp)->aggregate_N();
                          } catch (runtime_error &) {
                              cout << "Fail in aggregate_N" << endl;
                              return false;
                          }
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

                        // ... / W_per_szgroup
                        vector <double>W_per_szgroup=populations.at(sp)->get_weight_at_szgroup();
                        for(unsigned int sz = 0; sz < W_per_szgroup.size(); sz++)
                        {

                            popstats  << W_per_szgroup.at(sz)  << " " ;
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
                        //cout << "szgroup is " << i  << " " << endl ;
                        //cout << "tot_N_at_szgroup is " << populations.at(sp)->get_tot_N_at_szgroup().at(i)  << " " << endl ;
                        //cout << "maturity_at_szgroup is " << populations.at(sp)->get_maturity_at_szgroup().at(i)  << " " << endl ;
                        //cout << "weight_at_szgroup is " << populations.at(sp)->get_weight_at_szgroup().at(i)  << " kg" << endl ;

                        popstats  << SSB_per_szgroup.at(i) << " " ;
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
                           // first, compute fbar, whatever the management regime will be...
                           double fbar_py=0.0;
                           fbar_py= populations.at(sp)->compute_fbar();
                           populations.at(sp)->set_fbar(fbar_py);

                           if(dyn_alloc_sce.option(Options::TACs))
                            {
                               // compute a TAC for y+1 from a short-term forecast (STF)
                               // and a long-term management plan (LTMP)


                               double multiOnTACconstraint;
                               if(dyn_alloc_sce.option(Options::twoFoldTACconstr))
                                  {
                                  multiOnTACconstraint=2.0;
                               } else{
                                  multiOnTACconstraint=1.0; // default
                               }


                               if(dyn_alloc_sce.option(Options::LTMP)){

                                   try {
                                       bool er=computeTAC(populations, sp, tstep, multiOnTACconstraint, 1);
                                     } catch (runtime_error &) {
                                         cout << "Fail in computeTAC LTMP" << endl;
                                         return false;
                                     }
                               } else{
                                   if(dyn_alloc_sce.option(Options::FMSY)){

                                       try {
                                           bool er=computeTAC(populations, sp, tstep, multiOnTACconstraint, 2);
                                         } catch (runtime_error &) {
                                             cout << "Fail in computeTAC FMSY" << endl;
                                             return false;
                                         }
                                    } else{
                                       if(dyn_alloc_sce.option(Options::FMSYrange)){
                                           try {
                                               bool er=computeTAC(populations, sp, tstep, multiOnTACconstraint, 3);
                                             } catch (runtime_error &) {
                                                 cout << "Fail in computeTAC FMSYrange" << endl;
                                                 return false;
                                             }
                                       } else{
                                           try {
                                               bool er=computeTAC(populations, sp, tstep, multiOnTACconstraint, 4); // statuquo is default
                                             } catch (runtime_error &) {
                                                 cout << "Fail in computeTAC statuquo" << endl;
                                                 return false;
                                             }
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
                                   double TAC_y_plus_1;
                                   double TAC_y;
                                   if(tstep==8761) // year2
                                   {
                                      TAC_y_plus_1    = ts_tac.at(ts_tac.size()-2);
                                      TAC_y           = ts_tac.at(0);
                                   }
                                   else
                                   {
                                       TAC_y_plus_1    = ts_tac.at(ts_tac.size()-1);
                                       TAC_y           = ts_tac.at(ts_tac.size()-2);

                                   }
                                   double a_multiplier = TAC_y_plus_1 / TAC_y;
                                   if(TAC_y_plus_1==10000) a_multiplier=1; // edge case for non-binding TAC set in computeTAC()
                                   populations.at(sp)->set_oth_land_multiplier (a_multiplier);
                                   cout<< "The multiplier for oth_land on "<< sp << " in consequence of the TAC change is..." <<  a_multiplier << endl;
                                   cout<< "since TAC_y_plus_1 is " <<  TAC_y_plus_1 << endl;
                                   cout<< "since TAC_y is " <<  TAC_y << endl;
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
                                populations.at(sp)->export_popdyn_annual_indic(popdyn_annual_indic, tstep, dyn_alloc_sce);

                                // RE-INIT....
                                populations.at(sp)->clear_tot_F_at_age();
                                populations.at(sp)->clear_tot_M_at_age();
                                //=> obviously, F restart from 0 each year...
                                cout << "Adding to landings_at_end_of_years this pop "<< sp << ": " << populations.at(sp)->get_landings_so_far() << endl;
                                populations.at(sp)->add_to_landings_at_end_of_years(populations.at(sp)->get_landings_so_far());
                                populations.at(sp)->set_landings_so_far(1.0);
                                populations.at(sp)->reset_landings_so_far_per_nation();
                                populations.at(sp)->clear_tot_C_at_szgroup();
                                populations.at(sp)->clear_tot_D_at_szgroup();

                        }
                        else
                        {




                        } // end else



               cout << ".....monthly pop model (possibly add recruits, dispatch over nodes, apply MP) for pop " << sp  << "...ok" << endl;

               }
               else
               {
                       outc(cout << sp << ": implicit pop => no dynamic simulated..." << endl);
                       // apply only at the beginning of the year (this is maybe not always relevant...)
                       if(is_tacs && binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
                          {
                          //cout << "1- Current global tac for this pop " << sp << "is " << populations.at(sp)->get_tac()->get_current_tac() << endl;
                          double tac_y_plus_1 = populations.at(sp)->get_tac()->get_current_tac();

                          populations.at(sp)->get_tac()->add_tac_to_ts(tac_y_plus_1, -1);

                          //cout << "2- Current global tac for this pop " << sp << "is " << populations.at(sp)->get_tac()->get_current_tac() << endl;;

                          for (unsigned int vsl =0; vsl < vessels.size(); vsl ++)
                             {
                             vessels.at(vsl)->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, sp, 1, 0.0);
                             }

                          }
                    }

                }



    //...and update some indicators (i.e. cpue) and reinit monthly pop indicators on nodes after use
    int is_start_quarter = binary_search(tsteps_quarters.begin(), tsteps_quarters.end(), tstep);
    for (unsigned int n = 0; n < nodes.size(); n++)
    {
           // the cpue_per_pop_per_met is computed on the fly as a running average each time there are some do_catch() 
           // the cpue_per_pop_per_met is cleared each quarter.
           // but at the end of each month, cumcatch and cumeffort are reset because are being used in some other context (e.g. to compute oth_land per month)
           //cout << "cumcatch per pop " << sp << " and met 1 this month on node " << nodes.at(n)->get_idx_node() << " is " << 
           //    nodes.at(n)->get_cumcatches_per_pop_per_met_this_month().at(sp).at(1) << endl;
           nodes.at(n)->clear_cumcatches_per_pop_per_met_this_month(); // reinit after use
           //cout << "cumeffort per pop " << sp << " and met 1 this month on node " << nodes.at(n)->get_idx_node() << " is " <<
           //    nodes.at(n)->get_cumeffort_per_pop_per_met_this_month().at(sp).at(1) << endl;
           nodes.at(n)->clear_cumeffort_per_pop_per_met_this_month(); // reinit after use
           //cout << "cpue per pop " << sp << " and met 1 this month on node " << nodes.at(n)->get_idx_node() << " is " <<
           //    nodes.at(n)->get_cpue_per_pop_per_met_this_month().at(sp).at(1) << endl;
           if (is_start_quarter) nodes.at(n)->clear_cpue_per_pop_per_met_this_month(); // reinit after use

    }



    // apply only at the beginning of the year (this is maybe not always relevant...)
    if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
    {
        // export spatial distribution of biomass pop on nodes for mapping e.g. in GIS
        // pay attention to compare a start of a year with another start of a year...(because avai key is quarter spe)
        if (export_vmslike) {
            for (unsigned int n=0; n<nodes.size(); n++)
            {
                //nodes[n]->export_popnodes(popnodes_end, init_weight_per_szgroup, tstep);
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



    // FOR THE MAP WIDGET: update the populations values

    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {
           dout(cout << "..... compute totNs and totWs for pop " << sp << endl);
           int name_pop =populations.at(sp)->get_name();
           cout << "pop" << name_pop <<": update the populations values for the map widget" << endl;
           vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();
           vector <double> weight_at_szgroup = populations.at(sp)->get_weight_at_szgroup();

           for(unsigned int n=0; n<a_list_nodes.size(); n++)
           {
              //  compute totNs and totWs
              auto const &ns =a_list_nodes.at(n)->get_Ns_pops_at_szgroup(name_pop);
              double totN_this_pop =0.0;
              double totW_this_pop =0.0;
              for(unsigned int sz = 0; sz < ns.size(); sz++)
              {
                 totN_this_pop+= ns[sz];
                 totW_this_pop+= ns[sz] * weight_at_szgroup.at(sz);
              }
              a_list_nodes.at(n)->set_totNs_per_pop(name_pop, totN_this_pop);
              a_list_nodes.at(n)->set_totWs_per_pop(name_pop, totW_this_pop);



             // Update the "choking" area extend (pblm here of double counting...but too costly to avoid for now....)
             if(populations.at(sp)->get_is_choking_fisheries()) a_list_nodes.at(n)->set_nbchoked(a_list_nodes.at(n)->get_nbchoked() + 1);

           }
         }
    }


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
        nodes.at(n)->export_popnodes_nbchoked(popnodes_nbchoked, tstep);
        if(dyn_alloc_sce.option(Options::fishing_credits)) nodes.at(n)->export_popnodes_tariffs(popnodes_tariffs, tstep);
        // initially it was tstep == 34321 is 1st december 4th year. Now it is year 5
        if ((a_year_i == 3 || a_year_i == 8) && a_month_i == 12) nodes.at(n)->export_popnodes(popnodes_inc, init_weight_per_szgroup, tstep); // large size output

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


    if(dyn_pop_sce.option(Options::modelBenthosInN))
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

        // a recovery of the benthos biomasses on node on monthly basis
        // (ONLY IF NOT THE LONGEVITY APPROACH, BECAUSE LONGEVITY IS ON HOURLY BASIS)
        if(!dyn_pop_sce.option(Options::modelBenthosInLongevity))
        {
           for (unsigned int b=0; b<benthoss.size(); b++)
           {
            benthoss.at(b)->recover_benthos_tot_biomass_per_funcgroup(0);
           }
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
        guiSendUpdateCommand(popnodes_nbchoked_filename, tstep);
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

    //vector <double> a_tot_N_at_szgroup_here = populations.at(1)->get_tot_N_at_szgroup();
   // for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
   // cout << "tstep " << tstep << " AT THE END OF BIOL: a_tot_N_at_szgroup[" << sz << "]  here  is "<< a_tot_N_at_szgroup_here[sz]  << endl;

if(dyn_alloc_sce.option(Options::DEBUG_TAC) && will_stop==1)
  {
    cout << "Stop here because DEBUG_TAC Option is ON" << endl;

    //PAUSE
    int aa;
    cin >> aa;
  }

return true;
}




