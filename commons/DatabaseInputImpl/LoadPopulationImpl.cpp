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



#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Population.h"

#include "PopulationsLoader.h"

#include <iostream>

#define NBSZGROUP 14
#define NBAGE 11                 // nb age classes max
#define SEL_NBSZGROUP 5             // DEPRECATED - according to the R glm on cpue (see R code)

bool DatabaseModelLoader::loadPopulations(int year)
{
    PopulationsLoader loader(*p->db);
    auto pops = loader.loadPopulationBaseData();

 
    if (year == 1 && model().month() == 1) {

        
        // set up some node-related attributes informed from populations
        int nbmets = 100; // CAUTION: for now we don´t care as a large value will be resized. but best to ultimately include it to config()
        for (unsigned int i = 0; i < model().nodes().size(); i++) {
            model().nodes().at(i)->init_Ns_pops_at_szgroup(model().config().nbpops, NBSZGROUP, nbmets);
            model().nodes().at(i)->init_avai_pops_at_selected_szgroup(model().config().nbpops, SEL_NBSZGROUP);
        }



       auto populations = vector<Population *>();
       std::transform(pops.begin(), pops.end(), std::back_inserter(populations),
                   [this](PopulationsLoader::PopulationData const &d) {
                       std::cout << "Create: " << d.a_name << " " << d.a_pop_name << "\n";
                       auto pop = new Population(
                               d.a_name,
                               d.a_pop_name,
                               d.avai0_beta, d.avai2_beta, d.avai3_beta, d.avai5_beta, d.avai7_beta,

                               std::move(d.selected_szgroups),
                               std::move(d.init_tot_N_at_szgroup),
                               std::move(d.init_prop_migrants_in_tot_N_at_szgroup),
                               std::move(d.init_fecundity_at_szgroup),
                               std::move(d.init_weight_at_szgroup),
                               std::move(d.init_comcat_at_szgroup),
                               std::move(d.init_maturity_at_szgroup),
                               std::move(d.init_M_at_szgroup),
                               std::move(d.init_proprecru_at_szgroup),

                               std::move(d.param_sr),
                               std::move(d.full_spatial_availability),
                               std::move(d.field_of_coeff_diffusion_this_pop),
                               std::move(d.oth_land),
                               std::move(d.oth_land_map_per_met),
                               std::move(d.overall_migration_fluxes),
                               std::move(d.relative_stability_key),

                               std::move(d.percent_szgroup_per_age_matrix),
                               std::move(d.percent_age_per_szgroup_matrix),
                               std::move(d.growth_transition_matrix),

                               model().nodes(),
                               std::move(d.fbar_ages_min_max),
                               std::move(d.init_tac),

                               d.tac_percent_simulated,
                               d.hyperstability_param,
                               d.a_calib_cpue_multiplier,
                               d.a_calib_weight_at_szgroup
                       );

                       return pop;
                   });

       model().setPopulations(std::move(populations));

       std::cout << "Nodes: " << model().nodes().size() << "\n";
  
       std::cout << " Population creator() from db...done " << endl;

       
       
       // CALL SOME SETTERS-------------------
       vector<double> global_quotas_uptake;
       vector<vector<types::NodeId> > vect_of_lst_idx_nodes_per_pop_vov(model().populations().size());

       for (auto pop : model().populations())
       {
           int sp = pop->get_name();
       
           global_quotas_uptake.push_back(0.0);



           if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(), sp))
           {
               outc(cout << "inform avai on nodes " << endl);

               // get the vector of nodes of presence for this pop (an optimization to avoid looping over all nodes...)
               outc(cout << "first find the list of nodes with presence for this pop (this quarter)..." << endl);
               auto lower_pop = lst_idx_nodes_per_pop.lower_bound(sp); // TODO: extract lst_idx_nodes this pop from nodeid in pop->get_full_spatial_availability() instead!!!
               auto upper_pop = lst_idx_nodes_per_pop.upper_bound(sp);
               for (multimap<int, types::NodeId>::iterator a_pos = lower_pop; a_pos != upper_pop; a_pos++) {
                   vect_of_lst_idx_nodes_per_pop_vov.at(sp).push_back(a_pos->second);
               }

               outc(cout << "...then attach avai to each node for this pop (this quarter)" << endl);
               // init avai on each node (we know the presence...) for this pop for selected szgroup
               vector<types::NodeId> nodes_with_presence = vect_of_lst_idx_nodes_per_pop_vov.at(sp);
               multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = pop->get_full_spatial_availability();
             
               for (unsigned int n = 0; n < nodes_with_presence.size(); n++)
               {
                   dout(cout << ".");
                   auto spat_avai_this_pop_this_node = find_entries(avai_szgroup_nodes_with_pop,
                       nodes_with_presence.at(n));

                   vector<double> spat_avai_per_selected_szgroup;
                   vector<int> selected_szgroups = populations.at(sp)->get_selected_szgroups();
                   for (int sz = 0; sz < spat_avai_this_pop_this_node.size(); ++sz)
                   {
                       auto it = find(selected_szgroups.begin(), selected_szgroups.end(), sz);
                       if (it != selected_szgroups.end())
                       {
                           spat_avai_per_selected_szgroup.push_back(spat_avai_this_pop_this_node.at(sz));
                       }
                   }
                   if (!spat_avai_per_selected_szgroup.empty())
                   {
                       model().nodes().at(nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(sp,
                           spat_avai_per_selected_szgroup);
                   }
                   else
                   {
                       // inconsistence between lst_idx_nodes and avai files if this happen...
                       outc(cout << nodes_with_presence.at(n));
                   }

               }

               // check
               /*
              outc(cout << "avai at selected szgroup for the pop " << sp << " on a given node xx:" << endl); // used in do_catch != the one used in distributeN()
               vector<double> avai_pops_at_selected_szgroup = nodes[792]->get_avai_pops_at_selected_szgroup(sp);
               vector<double>::iterator szgroup = avai_pops_at_selected_szgroup.begin();
               for( ; szgroup != avai_pops_at_selected_szgroup.end(); szgroup++)
               {
                  outc(cout << *szgroup << " " );
               }
              outc(cout << endl);

               // check
              outc(cout << "tot N at szgroup for the pop " << sp << "on a given node xx:" << endl);
               vector<double> tot_N_at_szgroup = populations[sp]->get_tot_N_at_szgroup();
               vector<double>::iterator szgroup2 = tot_N_at_szgroup.begin();
               for( ; szgroup2 != tot_N_at_szgroup.end(); szgroup2++)
               {
                  outc(cout << *szgroup << " " );
               }
              outc(cout << endl);
               */

               outc(cout
                   << "if you have a problem of out of range here then check if you forgot a blank at the end of N_at_szgroup.dat! "
                   << endl);


               model().setPopulations(std::move(populations));
               model().setGlobalQuotasUptake(std::move(global_quotas_uptake));
               //model().setInitWeightPerSzgroup(std::move(init_weight_per_szgroup));
               //model().set_species_interactions_mortality_proportion_matrix(species_interactions_mortality_proportion_matrix);
           }   // end implicit pop
       }


    }
    else  // RE-LOAD POPULATIONS:
    {

       // tstep is >1
       cout << "redispatch the population over its spatial extent...." << endl;

       // aggregate from nodes to set the tot_N_at_szgroup per pop
       for (auto pop : model().populations())
       {
           int sp = pop->get_name();

           // aggregate from nodes (caution: do it before changing of list_nodes)
           if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(), sp))
           {

                       /*
                       if(sp==1){
                           vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(sp)->get_tot_N_at_szgroup();
                           for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                            cout << "BEFORE AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                       }
                       */
                       // get total N from summing up N over nodes
                       pop->aggregate_N();

                       /*
                       if(sp==1){
                           vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(sp)->get_tot_N_at_szgroup();
                           for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                            cout << "AFTER AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                       }
                       */

           }
       }
       cout << "aggregate_N over all pops....done" << endl;

       vector <std::string> type_of_avai_field_to_read;
       for (unsigned int ip = 0; ip < model().populations().size(); ip++)
       {
                   type_of_avai_field_to_read.push_back("");
       }

       if (model().scenario().dyn_pop_sce.option(Options::nbcpCoupling)) {
                   string a_command_for_R;

                   for (unsigned int pp = 0; pp < model().populations().size(); pp++) {
                       if (binary_search(model().config().nbcp_coupling_pops.begin(),
                           model().config().nbcp_coupling_pops.end(), pp)) {
                           type_of_avai_field_to_read.at(pp) = "_updated";

                           stringstream out;
                           out << pp;
                           string a_pop = out.str();

                           stringstream outtstep;
                           outtstep << model().timestep();
                           string atstep = outtstep.str();
#if defined(_WIN32)
                           cout << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?" << endl;
                           a_command_for_R = "Rscript .\\interactiverscripts\\nbcp_displace_coupling_part02.r " + a_pop + " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() + " " + model().scenario().a_graph_name;
                           cout << "executing " << a_command_for_R << endl;
                           system(a_command_for_R.c_str());
#else
                           cout << "nbcp_coupling...done" << endl;
                           // caution with HPC, annoying lower cases in file names and paths required!
                           a_command_for_R =
                               "Rscript " + p->inputfolder + "/interactiverscripts/nbcp_displace_coupling_part02.r " +
                               a_pop + " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() +
                               " " + model().scenario().a_graph_name;
                           system(a_command_for_R.c_str());
#endif
                       }  // end nbcp coupling pops
                   }  // end pop
       }


       if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on) ||
                   model().scenario().dyn_pop_sce.option(Options::avai_updater_on)) 
       {


                   // alter the availability field, if required
                   for (unsigned int pp = 0; pp < model().populations().size(); pp++) {
                       if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(),
                           pp)) {
                           stringstream out;
                           out << pp;
                           string a_pop = out.str();

                           stringstream out2;
                           out2 << model().scenario().nrow_coord;
                           string a_nrow_coord = out2.str();

                           string a_command;
                           string a_command_for_R;
                           stringstream outtstep;
                           outtstep << model().timestep();
                           string atstep = outtstep.str();
                           // the system command line
#if defined(_WIN32)
                           if (model().scenario().dyn_pop_sce.option(Options::avai_updater_on) && model().timestep() > 744) {
                               // note that nothing is done before end of 1st month (745) to get enough catch data for an update
                               type_of_avai_field_to_read.at(pp) = "_updated";
                               //system("dir");
                               // caution with HPC, annoying lower cases in file names and paths required!
                               cout
                                   << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?"
                                   << endl;
                               a_command_for_R =
                                   "Rscript .\\interactiverscripts\\input2avaiupdater.r " + a_pop + " " + atstep +
                                   " " + p->folder_name_parameterization + " " + model().nameSimu() + " " + model().scenario().a_graph_name;
                               //a_command_for_R = "R CMD BATCH .\\interactiverscripts\\input2avaiupdater.r "+a_pop+" "+atstep;
                               cout << "look after " << a_command_for_R << endl;
                               cout << "This supposes StockId " << a_pop
                                   << " is informed in displace_input_for_data_merger.csv input file" << endl;
                               system(a_command_for_R.c_str());
                               a_command = "avaifieldupdater.exe -tstep " + atstep + " -f " + p->folder_name_parameterization + " -a " +
                                   p->inputfolder + " -s " + std::to_string(model().semester()) + " -graph " +
                                   model().scenario().a_graph_name + " -nr " + a_nrow_coord + " -dist 15 -shepard_p 0.5";
                               cout << "look after " << a_command
                                   << endl; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                               system(a_command.c_str());
                           }
                           if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on)) {
                               type_of_avai_field_to_read.at(pp) = "_shuffled";
                               a_command = "avaifieldshuffler.exe -f " + p->folder_name_parameterization + " -s " +
                                   std::to_string(model().semester()) + " -p " + a_pop;
                               cout << "look after " << a_command
                                   << endl; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                               system(a_command.c_str());
                           }
#else
                           if (model().scenario().dyn_pop_sce.option(Options::avai_updater_on) && model().timestep() > 744) {
                               type_of_avai_field_to_read.at(pp) = "_updated";
                               // caution with HPC, annoying lower cases in file names and paths required!
                               a_command_for_R =
                                   "Rscript " + p->inputfolder + "/interactiverscripts/input2avaiupdater.r " + a_pop +
                                   " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() + " " +
                                   model().scenario().a_graph_name;
                               system(a_command_for_R.c_str());
                               a_command =
                                   p->inputfolder + "/avaifieldupdatertool -tstep " + atstep + " -f " +
                                   p->folder_name_parameterization +
                                   " -a " + p->inputfolder + " -s " + std::to_string(model().semester()) + " -graph " +
                                   model().scenario().a_graph_name +
                                   " -nr " + a_nrow_coord + " -dist 30 -shepard_p 0.5";
                               system(a_command.c_str());
                           }
                           if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on)) {
                               a_command = p->inputfolder + "/avaifieldshufflertool -f " + p->folder_name_parameterization +
                                   " -s " +
                                   std::to_string(model().semester()) + " -p " + a_pop;
                               system(a_command.c_str());
                           }
                           cout << "avaifieldshuffler...done" << endl;
#endif

                       }
                   }
       }


       // then, clean up all nodes before changing of spatial avai
       // (necessary to remove any fish in now wrong locations)
       cout << "clear pops on nodes" << endl;
       for (unsigned int i = 0; i < model().nodes().size(); i++) {
                   model().nodes().at(i)->clear_pop_names_on_node();
                   model().nodes().at(i)->clear_Ns_pops_at_szgroup();
                   model().nodes().at(i)->clear_avai_pops_at_selected_szgroup();
       }
       cout << "clear pops on nodes...done" << endl;

       // RE-read for simModel->populations()
       for (unsigned int i = 0; i < model().populations().size(); i++) 
       {
           stringstream out;
           out << i;

           cout << "RE-read for population " << model().populations().at(i)->get_name() << " from " <<
                       p->folder_name_parameterization << " " << p->inputfolder << " " << type_of_avai_field_to_read.at(i)
                       << endl;

           auto full_avai_szgroup_nodes_with_pop = loadedData.vectmmapndparam1.at(i);
           model().populations().at(i)->set_full_spatial_availability(full_avai_szgroup_nodes_with_pop);


           multimap<int, double> overall_migration_fluxes = loadedData.vectmmapidparam1.at(i);
           model().populations().at(i)->set_overall_migration_fluxes(overall_migration_fluxes);

           // apply the overall migration loss fluxes (i.e. on the overall N at szgroup)
           if (!model().scenario().dyn_pop_sce.option(Options::stop_mig_35065) || model().timestep() < 35065)
           {
                       model().populations().at(i)->apply_overall_migration_fluxes(model().populations());
           }

           //then, re-set the list_nodes and the pop_names_on_node
           // from the new area distribution given by this new spatial avai
           vector<Node*> list_nodes;
           for (auto iter = full_avai_szgroup_nodes_with_pop.begin();
                       iter != full_avai_szgroup_nodes_with_pop.end();
                       iter = full_avai_szgroup_nodes_with_pop.upper_bound(iter->first))
           {
                       list_nodes.push_back(model().nodes()[iter->first.toIndex()]);
                       model().nodes()[iter->first.toIndex()]->set_pop_names_on_node(i);
                       //   check per node
                       //   vector <int> pop_names = nodes[ iter->first ]->get_pop_names_on_node();
                       //   cout << "Node " << iter->first << endl;
                       //   for(int p=0;p<pop_names.size();p++) cout<< pop_names.at(p) << " ";
                       //   cout << endl;
           }
           model().populations().at(i)->set_list_nodes(list_nodes);

           // add the current Ns to the vectors of vectors of the concerned nodes
           vector<double> tot_N_at_szgroup = model().populations().at(i)->get_tot_N_at_szgroup();

           /*if( simModel->populations().at(i)->get_name()==1){
                       vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(i)->get_tot_N_at_szgroup();
                       for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                          cout << "CHECK IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
           */


           for (unsigned int n = 0; n < list_nodes.size(); n++)
           {
                       list_nodes[n]->set_Ns_pops_at_szgroup(i, tot_N_at_szgroup);
                       dout(cout << list_nodes[n]->get_idx_node().toIndex() << " ");
           }
           dout(cout << endl);

           // distribute tot_N_at_szgroup on nodes knowing the avai spatial key
           // i.e. update the vectors of vectors Ns_pops_at_szgroup of the nodes as usual
           // divide on nodes according to avai
           if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(),
                       i))
           {
                       model().populations().at(i)->distribute_N();

                       //if(simModel->populations().at(i)->get_name()==1){
                       //    vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(i)->get_tot_N_at_szgroup();
                       //    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                       //      cout << "CHECK IN MAIN2: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                       //}

                       //...and compute the Ns on nodes at the start of this month!
                       for (unsigned int n = 0; n < model().nodes().size(); n++) 
                       {
                           model().nodes().at(n)->set_Ns_pops_at_szgroup_at_month_start(i, model().nodes().at(
                               n)->get_Ns_pops_at_szgroup(i));
                       }
           }


           vector<types::NodeId> nodes_with_presence = loadedData.vovn1.at(i);
           multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = loadedData.vectmmapndparam1.at(i);

           for (unsigned int n = 0; n < nodes_with_presence.size(); n++)
           {
                       dout(cout << ".");
                       auto spat_avai_this_pop_this_node = find_entries(avai_szgroup_nodes_with_pop,
                           nodes_with_presence.at(n));

                       vector<double> spat_avai_per_selected_szgroup;
                       vector<int> selected_szgroups = model().populations().at(i)->get_selected_szgroups();
                       for (int sz = 0; sz < spat_avai_this_pop_this_node.size(); ++sz) 
                       {
                           auto it = find(selected_szgroups.begin(), selected_szgroups.end(), sz);
                           if (it != selected_szgroups.end()) 
                           {
                               spat_avai_per_selected_szgroup.push_back(spat_avai_this_pop_this_node.at(sz));
                           }
                       }
                       if (!spat_avai_per_selected_szgroup.empty()) 
                       {
                           model().nodes().at(
                               nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(i,
                                   spat_avai_per_selected_szgroup);
                       }
                       else 
                       {
                           // inconsistence between lst_idx_nodes and avai files if this happen...
                           outc(cout << nodes_with_presence.at(n));
                       }

           }


       }
       dout(cout << "re-read data for this period...OK" << endl);





    }


    
    
    
    
    return true;
}

