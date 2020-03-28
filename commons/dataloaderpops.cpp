#include "dataloaderpops.h"
#include<iostream>

using namespace std;



Dataloaderpops::Dataloaderpops()
{

}

int Dataloaderpops::gettype()
{
    return(1);
}








int Dataloaderpops::features(std::shared_ptr<sql::Storage> indb,
                             const string &folder_name_parameterization,
                             const string &inputfolder,
                             PopSceOptions const &dyn_pop_sce,
                             DynAllocOptions const &dyn_alloc_sce,
                             string const &biolsce,
                             string const &fleetsce,
                             ParamsForLoad &paramsForLoad,
                             LoadedData &loadedData)
{



    this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce,
                             dyn_alloc_sce,
                             biolsce,
                             fleetsce,
                             paramsForLoad,
                             loadedData);
  cout << "Loading pops features" << endl;


  // paramsForLoad.sparam1; // a_month
  // paramsForLoad.sparam2;// a_quarter
  // paramsForLoad.sparam3; //a_semester
  // paramsForLoad.iparam1; //nbpops
  // paramsForLoad.iparam2; //NBAGE
  // paramsForLoad.iparam3; //NBSZGROUP




  map<int, string> pop_names;
  read_pop_names_in_string(pop_names, folder_name_parameterization, inputfolder);

  vector<string > type_of_avai_field_to_read(paramsForLoad.iparam1);

  for (int st=0; st < paramsForLoad.iparam1; st++)
  {
     type_of_avai_field_to_read.at(st) ="";
  }
  string str_rand_avai_file="baseline"; // deprecated?
  // by default, will use the initial avai input

  // read the pop-specific betas related to the availability
  // szgroup0
  multimap<int, double> avai0_betas = read_avai_betas(paramsForLoad.sparam3, "0", folder_name_parameterization, inputfolder);
  // szgroup2
  multimap<int, double> avai2_betas = read_avai_betas(paramsForLoad.sparam3, "2", folder_name_parameterization, inputfolder);
  // szgroup3
  multimap<int, double> avai3_betas = read_avai_betas(paramsForLoad.sparam3, "3", folder_name_parameterization, inputfolder);
  // szgroup5
  multimap<int, double> avai5_betas = read_avai_betas(paramsForLoad.sparam3, "5", folder_name_parameterization, inputfolder);
  // szgroup7
  multimap<int, double> avai7_betas = read_avai_betas(paramsForLoad.sparam3, "7", folder_name_parameterization, inputfolder);

  // read other stuffs...
  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!
  cout << "Do the pop files init_pops_per_szgroup need a check?" << endl;
  multimap<int, double> init_pops_per_szgroup = read_init_pops_per_szgroup(folder_name_parameterization, inputfolder,
                                                                           biolsce);
  cout << "Do the pop files init_prop_migrants_pops_per_szgroup need a check?" << endl;
  multimap<int, double> init_prop_migrants_pops_per_szgroup = read_init_prop_migrants_pops_per_szgroup(
          folder_name_parameterization, inputfolder, biolsce);
  cout << "Do the pop files init_fecundity_per_szgroup need a check?" << endl;
  multimap<int, double> init_fecundity_per_szgroup = read_init_fecundity_per_szgroup(folder_name_parameterization,
                                                                                     inputfolder, biolsce);
  cout << "Do the pop files init_maturity_per_szgroup need a check?" << endl;
  multimap<int, double> init_maturity_per_szgroup = read_init_maturity_per_szgroup(folder_name_parameterization,
                                                                                   inputfolder, biolsce);
  cout << "Do the pop files init_weight_per_szgroupneed a check?" << endl;
  multimap<int, double> init_weight_per_szgroup = read_init_weight_per_szgroup(folder_name_parameterization,
                                                                               inputfolder, biolsce);
  cout << "Do the pop files init_comcat_per_szgroup need a check?" << endl;
  multimap<int, int> init_comcat_per_szgroup = read_init_comcat_per_szgroup(folder_name_parameterization,
                                                                            inputfolder);
  cout << "Do the pop files init_M_per_szgroup need a check?" << endl;
  multimap<int, double> init_M_per_szgroup = read_init_M_per_szgroup(folder_name_parameterization, inputfolder,
                                                                     biolsce);
  cout << "Do the pop files init_proprecru_per_szgroup need a check?" << endl;
  multimap<int, double> init_proprecru_per_szgroup = read_init_proprecru_per_szgroup(folder_name_parameterization,
                                                                                     inputfolder, biolsce);
  //cout << "Do the pop files lst_idx_nodes_per_pop need a check?" << endl;
  multimap<int, types::NodeId> lst_idx_nodes_per_pop = read_lst_idx_nodes_per_pop(paramsForLoad.sparam3,
                                                                                  folder_name_parameterization,
                                                                                  inputfolder, str_rand_avai_file);

  cout << "Do the pop files selected_szgroups need a check?" << endl;
  multimap<int, int> selected_szgroups = read_selected_szgroups_per_pop(folder_name_parameterization, inputfolder);
  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!


  // init
  // detect nb of selected szgroups arbitrary from pop0
  // and init the vector of vector avai object on node
  multimap<int, int>::iterator lselsz = selected_szgroups.lower_bound(0);
  multimap<int, int>::iterator uselsz = selected_szgroups.upper_bound(0);
  vector<int> selected_szgroups_pop0;
  for (multimap<int, int>::iterator pos = lselsz; pos != uselsz; pos++) {
      selected_szgroups_pop0.push_back(pos->second);
  }


  cout << "Does the pop file tac_percent_simulated need a check?" << endl;
  map<int, int> tac_percent_simulated = read_tac_percent_simulated(folder_name_parameterization, inputfolder);
  cout << "Does the pop file hyperstability_param need a check?" << endl;
  map<int, double> hyperstability_param = read_hyperstability_param(folder_name_parameterization, inputfolder);


  // input data, read proportion of natural mortality from other species when spatial co-occurences on node
  cout << "Do the species_interactions_mortality_proportion_matrix creation  need a check?" << endl;
  vector<vector<double> > species_interactions_mortality_proportion_matrix = read_species_interactions_mortality_proportion_matrix(
          paramsForLoad.iparam1, folder_name_parameterization, inputfolder, biolsce);



  // get the name of the pops
  // copy only unique elements of init_pops_per_szgroup into name_pops
  // DEADLY BUG: MAKE SURE THAT NO BLANK IS LEFT IN THE VERY END OF THE .DAT FILE...
  cout << "Do the name_pops creation  need a check?" << endl;
  vector<int> name_pops;
  for (multimap<int, double>::iterator iter = init_pops_per_szgroup.begin(); iter != init_pops_per_szgroup.end();
       iter = init_pops_per_szgroup.upper_bound(iter->first)) {
      name_pops.push_back(iter->first);
      cout << "pop " << iter->first << endl;

  }
  cout << "nb pops: " << name_pops.size() << endl;
  cout << "if you have a problem of overflow here then check if you forgot a blank at the end of N_at_szgroup.dat! "
       << endl;





  // initiate for loading loop over sp
  vector <double > vect_of_avai0_beta_v(name_pops.size());
  vector <double > vect_of_avai2_beta_v(name_pops.size());
  vector <double > vect_of_avai3_beta_v(name_pops.size());
  vector <double > vect_of_avai5_beta_v(name_pops.size());
  vector <double > vect_of_avai7_beta_v(name_pops.size());
  vector <vector<int> > vect_of_init_selected_szgroups_vov(name_pops.size());
  vector <vector<double> > vect_of_init_tot_N_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_prop_migrants_in_N_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_fecundity_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_maturity_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_weight_per_szgroup_vov(name_pops.size());
  vector <vector<int> > vect_of_init_comcat_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_M_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_proprecru_per_szgroup_vov(name_pops.size());

  //vector <multimap<types::NodeId, double> > vect_of_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
  vector <multimap<types::NodeId, double> > vect_of_full_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
  vector <multimap<types::NodeId, double> > vect_of_field_of_coeff_diffusion_this_pop_mmap(name_pops.size());
  vector <map<types::NodeId, double> > vect_of_oth_land_map(name_pops.size());
  vector <map<string, double> > vect_of_relative_stability_key_map(name_pops.size());
  vector <vector<vector<double> > > vect_of_growth_transition_matrix_vov(name_pops.size());
  vector <vector<vector<double> > > vect_of_percent_szgroup_per_age_matrix_vov(name_pops.size());
  vector <vector<vector<double> > > vect_of_percent_age_per_szgroup_matrix_vov(name_pops.size());
  vector <vector<double> > vect_of_param_sr_v(name_pops.size());
  vector <vector<double> > vect_of_fbar_ages_min_max_and_ftarget_this_pop_v(name_pops.size());
  vector <vector<double> > vect_of_tac_this_pop(name_pops.size());
  vector <multimap<int, double> > vect_of_overall_migration_fluxes_mmap(name_pops.size());
  vector<double> landings_so_far(name_pops.size());

  vector <vector<types::NodeId> > vect_of_lst_idx_nodes_per_pop_vov(name_pops.size());

  vector <string> popnames (name_pops.size());

  for (unsigned int sp = 0; sp < paramsForLoad.iparam1; sp++) {
      dout(cout << endl);

      popnames.at(sp) = pop_names[sp];
      cout << "pop_name: " << sp << ": " <<  popnames.at(sp) << endl;


      // avai0 beta for this particular pop
      multimap<int, double>::iterator lower_0 = avai0_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_0 = avai0_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_0; pos != upper_0; pos++)
          vect_of_avai0_beta_v.at(sp) = pos->second;

      // avai2 beta for this particular pop
      multimap<int, double>::iterator lower_2 = avai2_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_2 = avai2_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_2; pos != upper_2; pos++)
          vect_of_avai2_beta_v.at(sp)= pos->second;

      // avai3 beta for this particular pop
      multimap<int, double>::iterator lower_3 = avai3_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_3 = avai3_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_3; pos != upper_3; pos++)
          vect_of_avai3_beta_v.at(sp)= pos->second;

      // avai5 beta for this particular pop
      multimap<int, double>::iterator lower_5 = avai5_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_5 = avai5_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_5; pos != upper_5; pos++)
          vect_of_avai5_beta_v.at(sp)= pos->second;

      // avai7 beta for this particular pop
      multimap<int, double>::iterator lower_7 = avai7_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_7 = avai7_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_7; pos != upper_7; pos++)
          vect_of_avai7_beta_v.at(sp) = pos->second;

      // initial selected szgroups
      multimap<int, int>::iterator lower_init_selsz = selected_szgroups.lower_bound(sp);
      multimap<int, int>::iterator upper_init_selsz = selected_szgroups.upper_bound(sp);
      for (multimap<int, int>::iterator pos = lower_init_selsz; pos != upper_init_selsz; pos++)
          // convert in thousands
          vect_of_init_selected_szgroups_vov.at(sp).push_back(pos->second);

      // initial N for this particular pop
      multimap<int, double>::iterator lower_init = init_pops_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init = init_pops_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init; pos != upper_init; pos++)
          // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
          vect_of_init_tot_N_per_szgroup_vov.at(sp).push_back(pos->second * 1000);
      // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
      cout << "Caution: we remind you that DISPLACE expects input initial N in THOUSANDS...Did you check? " << endl;

      // initial prop_migrants for this particular pop
      multimap<int, double>::iterator lower_init_migrants = init_prop_migrants_pops_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_migrants = init_prop_migrants_pops_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_migrants; pos != upper_init_migrants; pos++)
          // convert in thousands
          vect_of_init_prop_migrants_in_N_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial fecundity for this particular pop
      multimap<int, double>::iterator lower_init_fec = init_fecundity_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_fec = init_fecundity_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_fec; pos != upper_init_fec; pos++)
          vect_of_init_fecundity_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial maturity for particular this pop
      multimap<int, double>::iterator lower_init_ma = init_maturity_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_ma = init_maturity_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_ma; pos != upper_init_ma; pos++)
          vect_of_init_maturity_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial weight for this particular pop
      multimap<int, double>::iterator lower_init_we = init_weight_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_we = init_weight_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_we; pos != upper_init_we; pos++)
          vect_of_init_weight_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial comcat for this particular pop
      multimap<int, int>::iterator lower_init_cc = init_comcat_per_szgroup.lower_bound(sp);
      multimap<int, int>::iterator upper_init_cc = init_comcat_per_szgroup.upper_bound(sp);
      for (multimap<int, int>::iterator pos = lower_init_cc; pos != upper_init_cc; pos++)
          vect_of_init_comcat_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial M for this particular pop
      multimap<int, double>::iterator lower_init_M = init_M_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_M = init_M_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_M; pos != upper_init_M; pos++)
          vect_of_init_M_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial proprecru for this particular pop
      multimap<int, double>::iterator lower_init_proprecru = init_proprecru_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_proprecru = init_proprecru_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_proprecru; pos != upper_init_proprecru; pos++)
          vect_of_init_proprecru_per_szgroup_vov.at(sp).push_back(pos->second);

      // input data, avai per szgroup on nodes and presence of the pop
      //vect_of_avai_szgroup_nodes_with_pop_mmap.at(sp) = read_avai_szgroup_nodes_with_pop(a_semester, sp,
      //                                                                                               folder_name_parameterization,
      //                                                                                               inputfolder,
      //                                                                                               str_rand_avai_file,
      //                                                                                               type_of_avai_field_to_read);
      vect_of_full_avai_szgroup_nodes_with_pop_mmap.at(sp) = read_full_avai_szgroup_nodes_with_pop(
                                                                                         paramsForLoad.sparam3, sp,
                                                                                                     folder_name_parameterization,
                                                                                                     inputfolder, str_rand_avai_file,
                                                                                                     type_of_avai_field_to_read);


      // get the vector of nodes of presence for this pop (an optimization to avoid looping over all nodes...)
      outc(cout << "first find the list of nodes with presence for this pop (this quarter)..." << endl);
      auto lower_pop = lst_idx_nodes_per_pop.lower_bound(sp);
      auto upper_pop = lst_idx_nodes_per_pop.upper_bound(sp);
      for (multimap<int, types::NodeId>::iterator a_pos = lower_pop; a_pos != upper_pop; a_pos++) {
          vect_of_lst_idx_nodes_per_pop_vov.at(sp).push_back(a_pos->second);
      }


      // input data
      if (dyn_pop_sce.option(Options::diffusePopN)) {
          cout << "read_field_of_coeff_diffusion_this_pop ..." << endl;
          vect_of_field_of_coeff_diffusion_this_pop_mmap.at(sp)  = read_field_of_coeff_diffusion_this_pop(paramsForLoad.sparam3, sp,
                                                                                     folder_name_parameterization,
                                                                                     inputfolder, biolsce);
      }

      // input data, read a other landings per node for this species
      vect_of_oth_land_map.at(sp)  = read_oth_land_nodes_with_pop(paramsForLoad.sparam3, paramsForLoad.sparam1, sp,
                                                                         folder_name_parameterization, inputfolder,
                                                                         fleetsce);

      vect_of_relative_stability_key_map.at(sp) = read_relative_stability_keys(paramsForLoad.sparam3, sp,
                                                                                folder_name_parameterization,
                                                                                inputfolder);

      // input data, growth transition, percent_szgroup_per_age_matrix
      vect_of_growth_transition_matrix_vov.at(sp) = read_growth_transition_matrix(sp, paramsForLoad.iparam3,
                                                                                       folder_name_parameterization,
                                                                                       inputfolder, biolsce);

      vect_of_percent_szgroup_per_age_matrix_vov.at(sp) = read_percent_szgroup_per_age_matrix(sp, paramsForLoad.iparam3,
                                                                                                   paramsForLoad.iparam2,
                                                                                                   folder_name_parameterization,
                                                                                                   inputfolder,
                                                                                                   biolsce);

      vect_of_percent_age_per_szgroup_matrix_vov.at(sp) = read_percent_age_per_szgroup_matrix(sp, paramsForLoad.iparam3,
                                                                                                   paramsForLoad.iparam2,
                                                                                                   folder_name_parameterization,
                                                                                                   inputfolder,
                                                                                                   biolsce);

      // input data, parameter for stock-recruitment relationship
      vect_of_param_sr_v.at(sp) = read_param_sr(sp, folder_name_parameterization, inputfolder, biolsce);

      // input data, fbar ages
      vect_of_fbar_ages_min_max_and_ftarget_this_pop_v.at(sp) = read_fbar_ages_min_max_and_ftarget(sp,
                                                                                                 folder_name_parameterization,
                                                                                                 inputfolder,
                                                                                                 biolsce);

      // input data, initial tac
      vect_of_tac_this_pop.at(sp) = read_initial_tac(sp, folder_name_parameterization, inputfolder);
      cout << "initial tac has been read correctly" << endl;


      if (dyn_alloc_sce.option(Options::TACs) && vect_of_tac_this_pop.at(sp).at(0) == 0) {
          cout << "WARNING: TACs Option is active: Consider informing a initial TAC value for pop" << sp
               << "and potentially other pops..." << endl;
          cout << "a fake, non binding value is filled in for now" << endl;
          vect_of_tac_this_pop.at(sp).at(0) = 100000; // tons
      }


      // input data, read migration fluxes in proportion per size group (if any)
      dout(cout << "read overall migration..." << endl);
      vect_of_overall_migration_fluxes_mmap.at(sp) = read_overall_migration_fluxes(paramsForLoad.sparam3, sp,
                                                                                     folder_name_parameterization,
                                                                                     inputfolder, biolsce);
      dout(cout << "overall migration has been read correctly" << endl);


      landings_so_far.push_back(1.0);



}





  //  export 
  // used in the Population() creator
  loadedData.vectsparam1=popnames;
  loadedData.vectdparam1=vect_of_avai0_beta_v;
  loadedData.vectdparam2=vect_of_avai2_beta_v;
  loadedData.vectdparam3=vect_of_avai3_beta_v;
  loadedData.vectdparam4=vect_of_avai5_beta_v;
  loadedData.vectdparam5=vect_of_avai7_beta_v;
  loadedData.vovi1= vect_of_init_selected_szgroups_vov;
  loadedData.vovd1= vect_of_init_tot_N_per_szgroup_vov;
  loadedData.vovd2= vect_of_init_prop_migrants_in_N_per_szgroup_vov;
  loadedData.vovd3= vect_of_init_fecundity_per_szgroup_vov;
  loadedData.vovd4= vect_of_init_weight_per_szgroup_vov;
  loadedData.vovi2= vect_of_init_comcat_per_szgroup_vov;
  loadedData.vovd5= vect_of_init_maturity_per_szgroup_vov;
  loadedData.vovd6= vect_of_init_M_per_szgroup_vov;
  loadedData.vovd7= vect_of_init_proprecru_per_szgroup_vov;
  loadedData.vovd8= vect_of_param_sr_v;
  loadedData.vectmmapndparam1=vect_of_full_avai_szgroup_nodes_with_pop_mmap;
  loadedData.vectmmapndparam2=vect_of_field_of_coeff_diffusion_this_pop_mmap;
  loadedData.vectmapndparam1=vect_of_oth_land_map;
  loadedData.vectmmapidparam1=vect_of_overall_migration_fluxes_mmap;
  loadedData.vectmapsdparam1=vect_of_relative_stability_key_map;
  loadedData.vovovd2=vect_of_percent_szgroup_per_age_matrix_vov;
  loadedData.vovovd3=vect_of_percent_age_per_szgroup_matrix_vov;
  loadedData.vovovd1=vect_of_growth_transition_matrix_vov;
  // nodes,
  loadedData.vovd9= vect_of_fbar_ages_min_max_and_ftarget_this_pop_v;
  loadedData.vovd10= vect_of_tac_this_pop;
  loadedData.mapiiparam1=tac_percent_simulated;
  loadedData.mapidparam1=hyperstability_param;
  loadedData.vectdparam6=paramsForLoad.vdparam1; // calib_cpue_multiplier;
  loadedData.vectdparam7=paramsForLoad.vdparam2;  // calib_weight_at_szgroup;
 
  // not used in the Population() creator
  loadedData.vectiparam1=selected_szgroups_pop0;
  loadedData.vovn1=vect_of_lst_idx_nodes_per_pop_vov; // nodes_with_presence

  // used in simulator main.cpp
  loadedData.mmapidparam_init_weight_per_szgroup=init_weight_per_szgroup;
  loadedData.vovd_species_interactions_mortality_proportion_matrix=species_interactions_mortality_proportion_matrix;

  loadedData.vectsparam2=type_of_avai_field_to_read;

return 0;
}

