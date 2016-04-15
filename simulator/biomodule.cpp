// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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
#include <options.h>
#include <readdata.h>
#include <helpers.h>
#include <ipc.h>
#include <profiler.h>

using namespace std;

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#ifdef PROFILE
extern AverageProfiler mPopExportProfile;
#endif

extern pthread_mutex_t glob_mutex;

static void lock()
{
    pthread_mutex_lock(&glob_mutex);
}

static void unlock()
{
    pthread_mutex_unlock(&glob_mutex);
}


int applyBiologicalModule(int tstep, string namesimu,
                          string namefolderinput, string namefolderoutput,	string pathoutput,
                          ofstream popdyn_N,
                          ofstream popdyn_F,
                          ofstream popdyn_annual_indic,
                          ofstream popdyn_test2,
                          ofstream popnodes_inc,
                          ofstream popnodes_impact,
                          ofstream popnodes_cumulcatches_per_pop,
                          ofstream popnodes_cumftime,
                          ofstream popnodes_cumsweptarea,
                          ofstream popnodes_cumcatches,
                          ofstream popnodes_tariffs,
                          ofstream export_individual_tacs,
                          ofstream popnodes_end,
                          ofstream benthosnodes,
                          int nbbenthospops,
                          FILE *pipe3,
                          FILE *pipe4,
                          bool use_gnuplot,
                          bool use_gui,
                          string popdyn_N_filename,
                          string popdyn_F_filename,
                          string popnodes_inc_filename,
                          string popnodes_end_filename,
                          string popnodes_impact_filename,
                          string popnodes_cumulcatches_per_pop_filename,
                          string popnodes_cumftime_filename,
                          string popnodes_cumsweptarea_filename,
                          string popnodes_cumcatches_filename,
                          string popnodes_tariffs_filename,
                          string popnodes_benthos_filename,
                          vector <int> tsteps_quarters, vector <int> tsteps_semesters,
                          vector <int> tsteps_years, vector <int> tsteps_months,
                          vector <int> implicit_pops,
                          vector <double> calib_oth_landings,
                          multimap<int, int> nodes_in_polygons,
                          bool is_tacs,
                          int export_vmslike,
                          int freq_do_growth,
                          multimap<int,double> init_weight_per_szgroup,
                          vector<Population* >& populations, vector<Node* >&nodes,
                          vector<Vessel* >& vessels,
                          PopSceOptions dyn_pop_sce, DynAllocOptions dyn_alloc_sce)
{







 // for the SMS coupling
 string sms_folder="sms-op-multi";
 ofstream SMS_N_in;
 ofstream SMS_F_in;



// init for SMS
int myints[] =			 // reuse the order of sp in the SMS file (i.e. cod, herring, sprat)
{
    10,3,7
};
vector<int> stock_numbers (myints, myints + sizeof(myints) / sizeof(int) );
int myints2[] =			 //  cod and herring in thousands, sprat in thousands and 10% in western baltic
{
    1000,1000,10000
};
vector<int> some_units (myints2, myints2 + sizeof(myints2) / sizeof(int) );
int myints3[] =			 //  cod and herring in thousands, sprat in thousands and 10% in western baltic
{
    8,8,8
};
vector<int> some_max_nb_ages (myints3, myints3 + sizeof(myints3) / sizeof(int) );

                         // entry point for the SMS model
if(dyn_pop_sce.option(Options::use_SMS) || dyn_pop_sce.option(Options::use_SMS_single))
{

   if(dyn_pop_sce.option(Options::use_SMS_single)) sms_folder="sms-op-single";

   if(tstep==0)
    {
        string filename="../displace_hpc_sh/"+sms_folder+"/"+namesimu+"/op_n.in";
        SMS_N_in.open(filename.c_str());
        write_SMS_OP_N_in_file(SMS_N_in, populations, stock_numbers, some_units, some_max_nb_ages);
        SMS_N_in.close();

        filename="../displace_hpc_sh/"+sms_folder+"/"+namesimu+"/op_f.in";
        SMS_F_in.open(filename.c_str());
        SMS_F_in << "# data by quarter, area, species and age"<< endl;

    }

}





//----------------------------------------//
//----------------------------------------//
// POP DYNAMICS --------------------------//
//----------------------------------------//
//----------------------------------------//

// CHECK...CHECK...CHECK...
// write done  pop N number in popdyn_test2
vector <int> sample_pops;
if(namefolderinput=="fake")
{
                         // for pop xx
    sample_pops.push_back(0);
                         // for pop xx
    sample_pops.push_back(1);
                         // for pop xx
    sample_pops.push_back(2);
}
else
{
                         // for pop xx
    sample_pops.push_back(0);
}
for (unsigned int sp=0; sp<sample_pops.size(); sp++)
{
    dout(cout  << "write down re-read pop in the popdyn_test2 file for checking...");
                         // get total N from summing up N over nodes
    populations.at( sample_pops.at(sp) )->aggregate_N();
    popdyn_test2 << setprecision(0) << fixed;
    // tstep / pop / tot N at szgroup
    popdyn_test2 << tstep << " " << sample_pops.at(sp) << " ";
    vector <double>tot_N_at_szgroup=populations.at( sample_pops.at(sp) )->get_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
    {
                         // output in thousands of individuals
        popdyn_test2  << tot_N_at_szgroup.at(sz) / 1000 << " " ;

    }
    popdyn_test2 << " " <<  endl;
}

// TO CHECK: SSB
// compute SSB
if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
{
    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {
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

            }
         // ...then, cumul for getting tot SSB (here in kilos)
         double SSB=0;
         for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
         {
         SSB +=  SSB_per_szgroup.at(i);
         }
         SSB= SSB/1000;			 //
         cout << "The SSB is " << SSB  << " tons" << endl ;
       }
    }
}



dout(cout  << "BEGIN: POP MODEL TASKS----------" << endl);
if(binary_search (tsteps_months.begin(), tsteps_months.end(), tstep))
{

    for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
        {

            int name_pop =populations.at(sp)->get_name();
           outc(cout << "apply other land on nodes..." << endl);
            vector <double> M_at_szgroup      = populations.at(sp)->get_M_at_szgroup();
            vector <double> weight_at_szgroup = populations.at(sp)->get_weight_at_szgroup();
            vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();
            map<int,double> map_oth           = populations.at(sp)->get_oth_land();

            outc(cout << "landings so far for this pop " << sp << ", before applying oth_land " <<
                populations.at(name_pop)->get_landings_so_far() << endl);

            // will be used in case of area_closure
            double cumul_oth_land_to_be_displaced=0.0;
            int a_source_node_idx=0;
            for(unsigned int n=0; n<a_list_nodes.size(); n++)
            {
                dout(cout << a_list_nodes.at(n)->get_idx_node() << " ");

                // a check
                vector <double> N_at_szgroup= a_list_nodes.at(n)->get_Ns_pops_at_szgroup(9);
                vector <double> removals_per_szgroup= a_list_nodes.at(n)->get_removals_pops_at_szgroup(9);
                if(a_list_nodes.at(n)->get_idx_node()==2436&& name_pop==9) {
                   outc(cout << "N_at_szgroup before oth_land" << endl);
                }
                for(unsigned int i=0; i<N_at_szgroup.size(); i++)
                {
                    if(a_list_nodes.at(n)->get_idx_node()==2436&& name_pop==9) {
                        outc(cout << N_at_szgroup.at(i) << endl);
                    }
                }
                if(a_list_nodes.at(n)->get_idx_node()==2436&& name_pop==9) {
                   outc(cout << "removals_per_szgroup before oth_land" << endl);
                }
                for(unsigned int i=0; i<removals_per_szgroup.size(); i++)
                {
                    if(a_list_nodes.at(n)->get_idx_node()==2436&& name_pop==9) {
                        outc(cout << removals_per_szgroup.at(i) << endl);
                    }
                }

                // apply "other" landings
                // (also accounting for a potential multiplier (default at 1.0))
                double oth_land_this_pop_this_node=
                    map_oth[ a_list_nodes.at(n)->get_idx_node() ]*
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

                dout (cout << "pop " << sp << " tentative catch in kg from others on this node " << a_list_nodes.at(n)->get_idx_node()
                    << ": " << oth_land_this_pop_this_node << endl);

                if(oth_land_this_pop_this_node!=0)
                {
                    // apply oth_land UNLESS this is a closed node!
                    // caution: quick and dirty solution because we dont know on which node
                    // the cumul will be re-affected....but we reassign the cumul to a node
                    // not farther than 200 km than a closed node.
                    // pseudocode: if a closed node then cumul otherwise apply oth_land + cumul if <200km otherwise apply oth_land only and continue the cumul
                    //
                    if (dyn_alloc_sce.option(Options::area_closure))
                    {

                        vector<int> polygons;
                        vector<int> polygon_nodes;

                        for (multimap<int, int>::iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
                        {
                            polygons.push_back(pos->first);
                            polygon_nodes.push_back(pos->second);
                            dout(cout  << " a polygon node is " << pos->second << endl);
                        }
                        sort (polygon_nodes.begin(), polygon_nodes.end());

                        if (binary_search (polygon_nodes.begin(), polygon_nodes.end(), a_list_nodes.at(n)->get_idx_node()))
                        {
                            // then, this is a closed node!
                            cumul_oth_land_to_be_displaced+=oth_land_this_pop_this_node;
                            dout(cout  << " this is a polygon node ! " << a_list_nodes.at(n)->get_idx_node() << endl);
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
                                dout(cout  <<  cumul_oth_land_to_be_displaced << " oth_land displaced on " << a_list_nodes.at(n)->get_idx_node()  << endl);

                         // reinit
                                cumul_oth_land_to_be_displaced=0.0;
                         // sinon, passe ton chemin....
                            }

                            // NO: this is not a closed node...so apply oth_land
                            // but also adding the previous cumul if suitable!
                            // (needed to impact the availability back)
                            vector <double> totN = populations.at(name_pop)->get_tot_N_at_szgroup();
                            a_list_nodes.at(n)->apply_oth_land(name_pop, oth_land_this_pop_this_node, weight_at_szgroup, totN);

                        }

                    }
                    else
                    {

                        // needed to impact the availability
                        vector <double> totN = populations.at(name_pop)->get_tot_N_at_szgroup();
                        a_list_nodes.at(n)->apply_oth_land(name_pop, oth_land_this_pop_this_node, weight_at_szgroup, totN);
                        dout(cout  << "oth_land this pop this node, check after potential correction (when total depletion): "<<  oth_land_this_pop_this_node << endl);

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
            dout(cout  << "pop " << name_pop << endl);
            vector <double>wsz = populations[name_pop]->get_weight_at_szgroup();
            for (unsigned int n=0; n<a_list_nodes.size(); n++)
            {
                dout(cout  << "node" << a_list_nodes.at(n)->get_idx_node() << endl);
                vector <double> N_at_szgroup_at_month_start= a_list_nodes.at(n)->get_Ns_pops_at_szgroup_at_month_start(name_pop);
                vector <double> N_at_szgroup= a_list_nodes.at(n)->get_Ns_pops_at_szgroup(name_pop);
                vector <double> removals_per_szgroup= a_list_nodes.at(n)->get_removals_pops_at_szgroup(name_pop);
                vector <double> pressure_per_szgroup_pop= a_list_nodes.at(n)->get_pressure_pops_at_szgroup(name_pop);

                // a check
                if(a_list_nodes.at(n)->get_idx_node()==2436 && name_pop==9)
                {
                   outc(cout << "N_at_szgroup_at_month_start" << endl);
                    for(unsigned int i=0; i<N_at_szgroup_at_month_start.size(); i++)
                    {
                       outc(cout << N_at_szgroup_at_month_start.at(i) << endl);
                    }
                   outc(cout << "removals_per_szgroup" << endl);
                    for(unsigned int i=0; i<removals_per_szgroup.size(); i++)
                    {
                       outc(cout << removals_per_szgroup.at(i) << endl);
                    }
                }

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

                //check
                if(a_list_nodes.at(n)->get_idx_node()==2436 && name_pop==9)
                {
                   outc(cout << "N_at_szgroup" << endl);
                    for(unsigned int i=0; i<N_at_szgroup.size(); i++)
                    {
                       outc(cout << N_at_szgroup.at(i) << endl);
                    }
                }

                if(tot_removals!=0)
                {

                    // check
                    if(a_list_nodes.at(n)->get_idx_node()==2436 && name_pop==9)
                    {
                       outc(cout << "pressure_per_szgroup_pop" << endl);
                        for(unsigned int i=0; i<pressure_per_szgroup_pop.size(); i++)
                        {
                           outc(cout << pressure_per_szgroup_pop.at(i) << endl);
                        }
                    }

                    // check
                    if(a_list_nodes.at(n)->get_idx_node()==2436 && name_pop==9)
                    {
                       outc(cout << "tot_removals " << tot_removals << endl);
                       outc(cout << "tot_B " << tot_B << endl);
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
                                    "on node " << a_list_nodes.at(n)->get_idx_node() << endl;
                                unlock();
                            }
                            if(tot_removals<0) {
                                lock();
                                cout << "negative tot_removals!! for this pop " << name_pop << " " <<
                                    "on node " << a_list_nodes.at(n)->get_idx_node() << endl;
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
                    if(export_vmslike && impact_on_pop!=0)
                    {
                        a_list_nodes.at(n)->export_popnodes_impact(popnodes_impact, tstep, name_pop);
                        a_list_nodes.at(n)->export_popnodes_cumulcatches_per_pop(popnodes_cumulcatches_per_pop, tstep, name_pop);
                    }
                }
                         // RE-INIT
                a_list_nodes.at(n)->clear_removals_pops_at_szgroup();
                a_list_nodes.at(n)->clear_impact_on_pops();
            }

            outc(cout << "landings so far for this pop " << sp << ", after applying oth_land " <<
                populations.at(name_pop)->get_landings_so_far() << endl);

            dout(cout  << endl);

            // At the aggregated population scale,
            // first, sum up the N over node and overwrite tot_N_at_szgroup....
            populations.at(sp)->aggregate_N();

            // check for consistency i.e. no gain in N !! (at least within the first semester, i.e. outside transition from growth...)
            //vector <double> end_month_Ns_at_szgroup_pop= populations.at(sp)->get_tot_N_at_szgroup();
            //vector <double> start_month_Ns_at_szgroup_pop=  populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
            //for(unsigned int szgroup=0; szgroup <start_month_Ns_at_szgroup_pop.size(); szgroup++)
            //{
            //    if(end_month_Ns_at_szgroup_pop.at(szgroup) > start_month_Ns_at_szgroup_pop.at(szgroup))
            //   {
            //        cout << "inconsistency in main() pop model for this pop " << populations.at(sp)->get_name() <<
            //             " for this szgroup " <<  szgroup <<endl;
            //        cout << "end_month_Ns_at_szgroup_pop is " << end_month_Ns_at_szgroup_pop.at(szgroup) << endl;
            //        cout << "while start_month_Ns_at_szgroup_pop is " << start_month_Ns_at_szgroup_pop.at(szgroup) << endl;
            //        int a_int;
            //        cin >> a_int; // pause
            //    }
            //}

            // then, compute F_at_age from F_at_szgroup as -log(N(t)/N(t-1))
            // knowing the ALK i.e.  prop szgroup in each age
            // (and no need of mortality_at_szgroup in the equ. because applied independently after)
            // remember that at init t=0, N_minus_1 = N...
            // N_minus_1 is updated AT THE VERY END of the pop model of this time step for the next
            dout(cout  << "compute this month the cumulated F_at_age on the whole pop..." << endl);
            populations.at(sp)->compute_tot_N_and_F_and_M_and_W_at_age();

        }
        else
        {
           outc(cout << sp << ": implicit pop => no dynamic simulated..." << endl);
        }
    }					 // end sp

                         // entry point for the SMS model
    if(dyn_pop_sce.option(Options::use_SMS)  || dyn_pop_sce.option(Options::use_SMS_single))
    {

        if(dyn_pop_sce.option(Options::use_SMS_single)) sms_folder="sms-op-single";

        // check for cod
        vector<double> Ns= populations.at(10)->get_tot_N_at_szgroup();
       outc(cout << "before" << endl);
        for(unsigned int sz=0; sz<Ns.size(); sz++)
        {
           outc(cout << Ns.at(sz) << " ");
        }
       outc(cout << endl);

        // convert and write as SMS input file
        if(binary_search (tsteps_quarters.begin(), tsteps_quarters.end(), tstep))
        {
            write_SMS_OP_F_in_file(SMS_F_in, populations, stock_numbers);
            for (unsigned int sp=0; sp<populations.size(); sp++)
            {
                if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
                {
                    populations.at(sp)->set_tot_F_at_age_last_quarter( populations.at(sp)->get_tot_F_at_age() );
                }
            }
        }

       outc(cout << " run SMS?"<< endl);
        // run (at the very end of the current year) one year SMS forecast each year
        // to deduce N_1stJan y+1 from N_1stJan y knowing Fs that occurred during the year y.
        // start the second year only
        if(binary_search (tsteps_years.begin(), tsteps_years.end(), tstep))
        {
            SMS_F_in.close();

            char cCurrentPath[FILENAME_MAX];
            char cSMSPath[FILENAME_MAX];

            // run SMS
            if(system(NULL))
            {
                printf ("Executing SMS...\n");
                if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
                {
                    return 1;
                }
               outc(cout << "The current working directory is" <<  cCurrentPath << endl);

                // change for the SMS folder where the SMS files are lying.
                #ifdef WINDOWS
                chdir ("C:\\Users\\fbas\\Documents\\GitHub\\displace_hpc_sh/");
                #else
                string aFolder = "/zhome/fe/8/43283/ibm_vessels/displace_hpc_sh/"+sms_folder+"/"+namesimu;
                if (chdir(aFolder.c_str()) == -1) {
                  cerr << "chdir failed!!" << endl;
                  // note that we cannot use ~/ibm_vessels in chdir!!!
                }
                #endif

                // check
                if (!GetCurrentDir(cSMSPath, sizeof(cSMSPath)))
                {
                    return 1;
                }
               outc(cout << "The SMS working directory is " << cSMSPath << endl);

                // the system command line
                #ifdef WINDOWS
                system("\"C:\\Users\\fbas\\Documents\\GitHub\\displace_hpc_sh\\op -maxfn 0 -nohess");
                #else
                string a_command = "~/ibm_vessels/displace_hpc_sh/"+sms_folder+"/"+namesimu+"/op -maxfn 0 -nohess";
                system(a_command.c_str());
                #endif
               outc(cout << "SMS done" << endl);

            }
            else
            {
                cerr << "pble calling a system command" << endl;
                exit (EXIT_FAILURE);
            }

            // return back to the initial path
            string aFolder2 = "/zhome/fe/8/43283/ibm_vessels/displace_hpc_sh";
                if (chdir(aFolder2.c_str()) == -1) {
                  cerr << "chdir failed!!" << endl;
                  // note that we cannot use ~/ibm_vessels in chdir!!!
                }

            // read .out SMS files and fill in
            read_SMS_OP_N_out_file(populations, stock_numbers, some_units, some_max_nb_ages, sms_folder, namesimu);

            // check
            if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
            {
                return 1;
            }
           outc(cout << "The current working directory is " << cCurrentPath << endl);

            // check for cod
            vector<double> Ns= populations.at(10)->get_tot_N_at_szgroup();
           outc(cout << "after" << endl);
            for(unsigned int sz=0; sz<Ns.size(); sz++)
            {
               outc(cout << Ns.at(sz) << " ");
            }
           outc(cout << endl);

        }
    }



    for (unsigned int sp=0; sp<populations.size(); sp++)
        {
            if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) &&  // not an implicit species
                    !binary_search (stock_numbers.begin(), stock_numbers.end(),  sp  )) // not a SMS species
            {
                vector<Node* > a_list_nodes       = populations.at(sp)->get_list_nodes();

                // apply M ---------
                // this is simply annual M divided by 12 because monthly time step...
                //cout << "apply M on the whole pop..." << endl;
                //populations.at(sp)->apply_natural_mortality(); // pble for using it if distribute_N() is not by month! i.e. the dead fish here are not removed from the nodes...
               outc(cout << "apply M on each node of the pop..." << endl);
                vector <double>  M_at_szgroup= populations.at(sp)->get_M_at_szgroup();
                for (unsigned int n=0; n<a_list_nodes.size(); n++)
                {
                    a_list_nodes.at(n)->apply_natural_mortality_at_node(sp, M_at_szgroup);
                }
                // then re-aggregate by summing up the N over node and overwrite tot_N_at_szgroup....
                populations.at(sp)->aggregate_N();

                // check...
                //vector <double> a_Ns_at_szgroup_pop=  populations.at(sp)->get_tot_N_at_szgroup();
                //for(unsigned int szgroup=0; szgroup <a_Ns_at_szgroup_pop.size(); szgroup++)
                //{
                //   outc(cout << "a_Ns_at_szgroup_pop just after applying M is " << a_Ns_at_szgroup_pop.at(szgroup) << endl);
                //}

                #ifdef _WIN32
                if(use_gnuplot)
                {

                    // first pop, szgroup 1,2,3
                    fflush( pipe3 );
                         // used when sometimes the simulation is too quick to be captured by gnuplot
                    Sleep( 50 );
                    fprintf(pipe3, "set terminal windows 1 size 400,400 position 100,500\n");
                    // note that possible warning messages from gnuplot are harmless...these messages are just
                    // related to the fact that gnuplot actually try to open the vmslike2.dat while this is too late...
                    string command = "plot '"+pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/popdyn_"+namesimu+".dat' every 24::0 using 1:3 with lines lc rgbcolor '#80A000', '"+pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/popdyn_"+namesimu+".dat' every 24::0 using 1:4 with lines lc rgbcolor '#74C6D5', 'outputs/"+namefolderinput+"/popdyn_"+namesimu+".dat' every 24::0 using 1:5 with lines lc rgbcolor '#A2CAD1'\n";
                         //plotting the .dat file
                    fprintf(pipe3, command.c_str());

                    // second pop, szgroup 1,2,3
                    fflush( pipe4 );
                    // used when sometimes the simulation is too quick to be captured by gnuplot:
                    Sleep( 50 );
                    fprintf(pipe4, "set terminal windows 2 size 400,400 position 100,1000\n");
                    string command2 = "plot '"+pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/popdyn_"+namesimu+".dat' every 24::1 using 1:3 with lines lc rgbcolor '#80A000', '"+pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/popdyn_"+namesimu+".dat' every 24::1 using 1:4 with lines lc rgbcolor '#74C6D5', 'outputs/"+namefolderinput+"/popdyn_"+namesimu+".dat' every 24::1 using 1:5 with lines lc rgbcolor '#A2CAD1'\n";
                         //plotting the .dat file
                    fprintf(pipe4, command2.c_str());

                }
                #endif

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
                    double sum_N_year_minus_1;
                    double sum_N_start_current_year;
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

for (unsigned int sp=0; sp<populations.size(); sp++)
    {
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) &&  // not an implicit species
                !binary_search (stock_numbers.begin(), stock_numbers.end(),  sp  )) // not a SMS species
        {

                // apply growth only by semester if want to be consistent with the timeframe of the survey data
                // apply growth consistently with the time frame chosen during the parameterisation because the matrix time specific

                // timing (update at 7 a.m.)
               int do_growth=0;
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
                   outc(cout<< "DO GROWTH TRANSITION: caution, the matrix is time-specific in the parameterisation" << endl);
                    populations[sp]->do_growth();
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

            // spread out the recruits
            // apply only by semester, to be consistent with the timeframe of survey data
            if(binary_search (tsteps_semesters.begin(), tsteps_semesters.end(), tstep))
            {
                // at the very end, then re-dispatch over nodes to re-dispatch the recruits over the nodes....
                populations.at(sp)->distribute_N();
            }

            //...and compute the Ns on nodes at the start of this month!
            for (unsigned int n=0; n<nodes.size(); n++)
            {
                nodes.at(n)->set_Ns_pops_at_szgroup_at_month_start(name_pop, nodes.at(n)->get_Ns_pops_at_szgroup(sp));
            }

            // check...
            //vector <double> a_start_month_Ns_at_szgroup_pop=  populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
            //for(unsigned int szgroup=0; szgroup <a_start_month_Ns_at_szgroup_pop.size(); szgroup++)
            //{
            //    cout << "a_start_month_Ns_at_szgroup_pop is " << a_start_month_Ns_at_szgroup_pop.at(szgroup) << endl;
            //}

            //... and export for plotting.
           outc(cout << "write down the popdyn...");
                         // N at szgroup
            populations.at(sp)->export_popdyn_N (popdyn_N, tstep);
                         // ...and F at age
            populations.at(sp)->export_popdyn_F (popdyn_F, tstep);




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



                               double multiOnTACconstraint;
                               if(dyn_alloc_sce.option(Options::twoFoldTACconstr))
                                  {
                                  multiOnTACconstraint=2.0;
                               } else{
                                  multiOnTACconstraint=1.0; // default
                               }


                               if(dyn_alloc_sce.option(Options::LTMP)) populations.at(sp)->compute_TAC(multiOnTACconstraint, 1);
                               if(dyn_alloc_sce.option(Options::FMSY)) populations.at(sp)->compute_TAC(multiOnTACconstraint, 2);

                               outc(cout<< "initialize individual vessel TAC for this coming year" << endl);
                                   // initialise the individual quota from global_TAC*percent_in_simu*percent_this_vessel
                                   for (unsigned int vsl =0; vsl < vessels.size(); vsl ++)
                                      {
                                       vessels.at(vsl)->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, sp, 1, 0.0);
                                      }
                                   outc(cout<< "compute the multiplier for oth_land in consequence of the TAC change" << endl);
                                   // to do next time oth_land will be applied: oth_land * TACy+1 / TACy
                                   vector<double> ts_tac  = populations.at(sp)->get_tac()->get_ts_tac();
                                   double TAC_y_plus_1    = ts_tac.at(ts_tac.size()-1);
                                   double TAC_y           = ts_tac.at(ts_tac.size()-2);
                                   populations.at(sp)->set_oth_land_multiplier (TAC_y_plus_1 / TAC_y);
                                   if(populations.at(sp)->get_oth_land_multiplier()!=
                                     // i.e. a trick to check if nan
                                    populations.at(sp)->get_oth_land_multiplier())
                                      {
                                      outc(cout << "stop: check the c++ code for oth_land_multiplier"<< endl);
                                       int ff;
                                       cin >>ff;

                                }
                               } else{ // if no TAC regime
                                  populations.at(sp)->set_oth_land_multiplier (1.0);
                               }

                                // export
                                // ...export the cpue and oth_land multiplier
                                populations.at(sp)->export_popdyn_annual_indic(popdyn_annual_indic, tstep);

                                // RE-INIT....
                                populations.at(sp)->clear_tot_F_at_age();
                                //=> obviously, F restart from 0 each year...
                                populations.at(sp)->set_landings_so_far(0);

                            //}
                            //else
                            //{
                             //}

                            // store N initial for the next year and reinit Fs
                            if(dyn_pop_sce.option(Options::use_SMS) || dyn_pop_sce.option(Options::use_SMS_single))
                            {
                                if(dyn_pop_sce.option(Options::use_SMS_single)) sms_folder="sms-op-single";

                                ofstream SMS_N_in;
                                string filename="../displace_hpc_sh/"+sms_folder+"/"+namesimu+"/op_n.in";
                                SMS_N_in.open(filename.c_str(), ios::trunc);
                                write_SMS_OP_N_in_file(SMS_N_in, populations, stock_numbers, some_units, some_max_nb_ages);
                                SMS_N_in.close();

                                ofstream SMS_F_in;
                                filename="../displace_hpc_sh/"+sms_folder+"/"+namesimu+"/op_f.in";
                                SMS_F_in.open(filename.c_str(), ios::trunc);

                            }

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
                             vessels.at(vsl)->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, sp, 1, 0.0);
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
            }
            if (use_gui) {
                popnodes_end.flush();
                guiSendUpdateCommand(popnodes_end_filename, tstep);
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
        if(export_vmslike) nodes.at(n)->export_popnodes_cumsweptarea(popnodes_cumsweptarea, tstep);
        nodes.at(n)->export_popnodes_cumcatches(popnodes_cumcatches, tstep);
        if(dyn_alloc_sce.option(Options::fishing_credits)) nodes.at(n)->export_popnodes_tariffs(popnodes_tariffs, tstep);
        if(export_vmslike) nodes.at(n)->export_popnodes(popnodes_inc, init_weight_per_szgroup, tstep); // large size output disabled if -e at 0
    }

    //...and export the benthos biomasses on node
    for (unsigned int n=0; n<nodes.size(); n++)
    {
        for(unsigned int funcgroup=0;funcgroup< (unsigned int)nbbenthospops; funcgroup++){
//                   nodes.at(n)->export_benthos_tot_biomass_per_funcgroup(benthosnodes, tstep, funcgroup);
        }
    }

    // a recovery of the benthos biomasses on node
    for (unsigned int n=0; n<nodes.size(); n++)
    {
        nodes.at(n)->recover_benthos_tot_biomass_per_funcgroup();
    }

    /* Flush and updates all statistics */
    if (use_gui) {
        popnodes_cumftime.flush();
        popnodes_cumsweptarea.flush();
        popnodes_cumcatches.flush();
        guiSendUpdateCommand(popnodes_cumftime_filename, tstep);
        guiSendUpdateCommand(popnodes_cumsweptarea_filename, tstep);
        guiSendUpdateCommand(popnodes_cumcatches_filename, tstep);
        guiSendUpdateCommand(popnodes_tariffs_filename, tstep);

        popnodes_impact.flush();
        guiSendUpdateCommand(popnodes_impact_filename, tstep);

        popnodes_cumulcatches_per_pop.flush();
        guiSendUpdateCommand(popnodes_cumulcatches_per_pop_filename, tstep);

        benthosnodes.flush();
        guiSendUpdateCommand(popnodes_benthos_filename, tstep);

        if (export_vmslike) {
            popnodes_inc.flush();
            guiSendUpdateCommand(popnodes_inc_filename, tstep);
        }

        popdyn_F.flush();
        guiSendUpdateCommand(popdyn_F_filename, tstep);

        popdyn_N.flush();
        guiSendUpdateCommand(popdyn_N_filename, tstep);
    }
#ifdef PROFILE
    mPopExportProfile.elapsed_ms();
#endif
}
dout(cout  << "END: POP MODEL TASKS----------" << endl);


return 0;
}



