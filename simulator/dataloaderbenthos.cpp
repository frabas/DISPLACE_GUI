#include "dataloaderbenthos.h"


Dataloaderbenthos::Dataloaderbenthos()
{

}

int Dataloaderbenthos::gettype()
{
    return(2);
}

void Dataloaderbenthos::features(const string &folder_name_parameterization,
                                 const string &inputfolder,
                                 PopSceOptions &dyn_pop_sce)
{
  this->Dataloader::features(folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce);
  cout << "Loading benthos features" << endl;



  // read estimates
  multimap<int, double> meanw_funcgr_per_node = read_meanw_funcgr_per_landscape(folder_name_parameterization,
                                                                                inputfolder);
  multimap<int, double> prop_funcgr_number_per_node;
  multimap<int, double> benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
  multimap<int, double> prop_funcgr_biomass_per_node;
  multimap<int, double> benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
  multimap<int, double> longevity_classes_condition_per_node;


  if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
      longevity_classes_condition_per_node = read_longevity_classes_condition_per_node(folder_name_parameterization,
                                                                                       inputfolder);
      benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr = read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(
              folder_name_parameterization, inputfolder);
  } else {
      if (dyn_pop_sce.option(Options::modelBenthosInN)) {
          prop_funcgr_number_per_node = read_prop_funcgr_number_per_node_per_landscape(folder_name_parameterization,
                                                                                       inputfolder);
          benthos_number_carrying_capacity_K_per_landscape_per_funcgr = read_benthos_number_carrying_capacity_K_per_landscape_per_funcgr(
                  folder_name_parameterization, inputfolder);
      } else {
          prop_funcgr_biomass_per_node = read_prop_funcgr_biomass_per_node_per_landscape(folder_name_parameterization,
                                                                                         inputfolder);
          benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr = read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(
                  folder_name_parameterization, inputfolder);
      }
  }


  multimap<int, double> recovery_rates_per_funcgr = read_logistic_recovery_rates_per_month_per_funcgr(
          folder_name_parameterization, inputfolder);

  multimap<int, double> habitat_deltas_per_pop = read_habitat_deltas_per_pop(folder_name_parameterization,
                                                                             inputfolder);



  // 2. sort and unique
  sort(graph_point_code_landscape.begin(), graph_point_code_landscape.end());
  std::vector<int>::iterator it;
  it = std::unique(graph_point_code_landscape.begin(), graph_point_code_landscape.end());
  graph_point_code_landscape.resize(std::distance(graph_point_code_landscape.begin(), it));
  int nbland = graph_point_code_landscape.size();

  // creation of a vector of benthos shared (one benthos shared per landscape)
  benthoss = vector<Benthos *>(nbland);

  outc(cout << "nb of marine landscapes " << nbland << endl);

  for (int landscape = 0; landscape < nbland; landscape++) {

      vector<double> init_meanw_funcgr_per_node;
      vector<double> init_prop_funcgr_number_per_node;
      vector<double> init_prop_funcgr_biomass_per_node;
      vector<double> init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
      vector<double> init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
      vector<double> init_recovery_rates_per_funcgr;
      vector<double> init_h_betas_per_pop;

      int a_marine_landscape = graph_point_code_landscape.at(landscape);

      outc(cout << "a marine landscape " << a_marine_landscape << endl);

      if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
          multimap<int, double>::iterator lower_it_lgy = longevity_classes_condition_per_node.lower_bound(0);
          multimap<int, double>::iterator upper_it_lgy = longevity_classes_condition_per_node.upper_bound(0);

          vector<double> a_vector;
          for (multimap<int, double>::iterator pos = lower_it_lgy; pos != upper_it_lgy; pos++) {
              a_vector.push_back(pos->second);
          }


          if (a_vector.size() != (size_t) nbbenthospops) {
              cout << "on node 0: " << " nb longevity class is " << a_vector.size() <<
                   ": error for benthos file: check the dims in longevity_classes_condition_per_node input file. kill, correct and re-run."
                   << endl;
              int aa;
              cin >> aa;
          }

          multimap<int, double>::iterator lower_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                  a_marine_landscape);
          multimap<int, double>::iterator upper_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                  a_marine_landscape);
          for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
              outc(cout << pos->second << endl);
              // logistic recovery rates for this group specific to this landscape
              init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
          }
      } else {
          if (dyn_pop_sce.option(Options::modelBenthosInN)) {
              multimap<int, double>::iterator lower_land = prop_funcgr_number_per_node.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_land = prop_funcgr_number_per_node.upper_bound(
                      a_marine_landscape);
              for (multimap<int, double>::iterator pos = lower_land; pos != upper_land; pos++) {
                  outc(cout << pos->second << endl);
                  // biomass per cell for this group specific to this landscape
                  init_prop_funcgr_number_per_node.push_back(pos->second);
              }

              multimap<int, double>::iterator lower_landddd = benthos_number_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_landddd = benthos_number_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                      a_marine_landscape);
              for (multimap<int, double>::iterator pos = lower_landddd; pos != upper_landddd; pos++) {
                  outc(cout << pos->second << endl);
                  // logistic recovery rates for this group specific to this landscape
                  init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
              }


          } else {

              multimap<int, double>::iterator lower_land2 = prop_funcgr_biomass_per_node.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_land2 = prop_funcgr_biomass_per_node.upper_bound(
                      a_marine_landscape);

              for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
                  outc(cout << "check this: " << pos->second << endl);
                  // biomass per cell for this group specific to this landscape
                  init_prop_funcgr_biomass_per_node.push_back(pos->second);
              }


              if (init_prop_funcgr_biomass_per_node.size() != (size_t) nbbenthospops) {
                  cout << a_marine_landscape << " nb funcgr is " << init_prop_funcgr_biomass_per_node.size() <<
                       ": error for benthos file: the file is likely to get an extra blank space here. stop, remove and rerun."
                       << endl;
                  int aa;
                  cin >> aa;
              }

              multimap<int, double>::iterator lower_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                      a_marine_landscape);
              for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
                  outc(cout << pos->second << endl);
                  // logistic recovery rates for this group specific to this landscape
                  init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
              }


          }
      }


      multimap<int, double>::iterator lower_land3 = meanw_funcgr_per_node.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_land3 = meanw_funcgr_per_node.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_land3; pos != upper_land3; pos++) {
          outc(cout << pos->second << endl);
          // biomass per cell for this group specific to this landscape
          init_meanw_funcgr_per_node.push_back(pos->second);
      }


      multimap<int, double>::iterator lower_landd = recovery_rates_per_funcgr.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_landd = recovery_rates_per_funcgr.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_landd; pos != upper_landd; pos++) {
          outc(cout << pos->second << endl);
          // logistic recovery rates for this group specific to this landscape
          init_recovery_rates_per_funcgr.push_back(pos->second);
      }

      multimap<int, double>::iterator lower_land2 = habitat_deltas_per_pop.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_land2 = habitat_deltas_per_pop.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
          outc(cout << pos->second << endl);
          // habitat_deltas_per_pop specific to this landscape
          init_h_betas_per_pop.push_back(pos->second);
      }


      // add e.g. 2 functional groups per shared
      // and init with an arbitrary biomass.
      // init_biomass will be distributed evenly among nodes
      // belonging to this particular landscape

      benthoss[landscape] = new Benthos(landscape,
                                        a_marine_landscape,
                                        nodes,
                                        init_prop_funcgr_biomass_per_node,
                                        init_prop_funcgr_number_per_node,
                                        init_meanw_funcgr_per_node,
                                        init_recovery_rates_per_funcgr,
                                        init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,
                                        init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr,
                                        is_benthos_in_numbers,
                                        is_benthos_in_longevity_classes,
                                        init_h_betas_per_pop,
                                        longevity_classes_condition_per_node
      );
      //out(cout << "marine landscape for this benthos shared is " << benthoss.at(landscape)->get_marine_landscape() << endl);
      //out(cout <<"...and the biomass this node this func. grp is "  << benthoss.at(landscape)-> get_list_nodes().at(0)-> get_benthos_tot_biomass(0) << endl);

  }

  // check
  for (unsigned int a_idx = 0; a_idx < nodes.size(); a_idx++) {
      dout(cout << "this node " << nodes.at(a_idx)->get_idx_node().toIndex() <<
                " nb func. gr. " << nodes.at(a_idx)->get_benthos_tot_biomass().size() << endl);

      if (nodes.at(a_idx)->get_benthos_tot_biomass().size() != (size_t) nbbenthospops) {
          cout
                  << "something wrong for benthos_tot_biomass here!...kill displace.exe and check consistency in landscape coding and benthos input files before trying again"
                  << endl;
          int aa;
          cin >> aa;
      }
  }

  // check the area distribution for benthos shared 0
  //vector<Node* > some_nodes= benthoss.at(0)-> get_list_nodes();
  //for(int a_idx=0; a_idx<some_nodes.size(); a_idx++){
  //    cout << some_nodes.at(a_idx)->get_idx_node() << endl;
  //}

  // check the biomasses
  vector<double> a_prop_funcgr_per_node = benthoss[0]->get_prop_funcgr_biomass_per_node();
  outc(cout << "check biomass per func. gr. for benthos shared 0  " << endl);
  for (unsigned int gr = 0; gr < a_prop_funcgr_per_node.size(); gr++) {
      outc(cout << a_prop_funcgr_per_node[gr] << " ");
  }
  outc(cout << endl);


}
