#include "dataloaderbenthos.h"




Dataloaderbenthos::Dataloaderbenthos()
{

}

int Dataloaderbenthos::gettype()
{
    return(2);
}

void Dataloaderbenthos::features(std::shared_ptr<sql::Storage> indb,
                                 const string &folder_name_parameterization,
                                 const string &inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 LoadedData & loadedData)
{
  this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce,
                             loadedData);
  cout << "Loading benthos features" << endl;


  bool is_benthos_in_numbers;
  bool is_benthos_in_longevity_classes;

  if (dyn_pop_sce.option(Options::modelBenthosInN)) {
      is_benthos_in_numbers = 1;
  } else {
      is_benthos_in_numbers = 0; // if not N then it impacts the benthos biomass by default
  }

  if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
      is_benthos_in_longevity_classes = 1;
  } else {
      is_benthos_in_longevity_classes = 0;
  }


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
  vector<int> all_landscapes;
  for (auto iter = habitat_deltas_per_pop.begin(); iter != habitat_deltas_per_pop.end();
       iter = habitat_deltas_per_pop.upper_bound(iter->first)) {
      all_landscapes.push_back(iter->first);
  }
  sort(all_landscapes.begin(), all_landscapes.end());
  std::vector<int>::iterator it;
  it = std::unique(all_landscapes.begin(), all_landscapes.end());
  all_landscapes.resize(std::distance(all_landscapes.begin(), it));
  int nbland = all_landscapes.size();

  outc(cout << "nb of marine landscapes " << nbland << endl);

  for (int landscape = 0; landscape < nbland; landscape++) {

      //loadedData.vectparam1 replacing vector<double> init_meanw_funcgr_per_node;
      //loadedData.vectparam2 replacing vector<double> init_prop_funcgr_number_per_node;
      //loadedData.vectparam3 replacing vector<double> init_prop_funcgr_biomass_per_node;
      //loadedData.vectparam4 replacing vector<double> init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
      //loadedData.vectparam5 replacing vector<double> init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
      //loadedData.vectparam6 replacing vector<double> init_recovery_rates_per_funcgr;
      //loadedData.vectparam7 replacing vector<double> init_h_betas_per_pop;

      int a_marine_landscape = all_landscapes.at(landscape);

      outc(cout << "a marine landscape " << a_marine_landscape << endl);

      if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
          multimap<int, double>::iterator lower_it_lgy = longevity_classes_condition_per_node.lower_bound(0);
          multimap<int, double>::iterator upper_it_lgy = longevity_classes_condition_per_node.upper_bound(0);

          vector<double> a_vector;
          for (multimap<int, double>::iterator pos = lower_it_lgy; pos != upper_it_lgy; pos++) {
              a_vector.push_back(pos->second);
          }


          //if (a_vector.size() != (size_t) nbbenthospops) {
          //    cout << "on node 0: " << " nb longevity class is " << a_vector.size() <<
          //         ": error for benthos file: check the dims in longevity_classes_condition_per_node input file. kill, correct and re-run."
          //         << endl;
          //    int aa;
          //    cin >> aa;
          //}

          multimap<int, double>::iterator lower_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                  a_marine_landscape);
          multimap<int, double>::iterator upper_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                  a_marine_landscape);
          for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
              outc(cout << pos->second << endl);
              // logistic recovery rates for this group specific to this landscape
              loadedData.vectparam5.push_back(pos->second);
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
                  loadedData.vectparam2.push_back(pos->second);
              }

              multimap<int, double>::iterator lower_landddd = benthos_number_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_landddd = benthos_number_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                      a_marine_landscape);
              for (multimap<int, double>::iterator pos = lower_landddd; pos != upper_landddd; pos++) {
                  outc(cout << pos->second << endl);
                  // logistic recovery rates for this group specific to this landscape
                  loadedData.vectparam4.push_back(pos->second);
              }


          } else {

              multimap<int, double>::iterator lower_land2 = prop_funcgr_biomass_per_node.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_land2 = prop_funcgr_biomass_per_node.upper_bound(
                      a_marine_landscape);

              for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
                  outc(cout << "check this: " << pos->second << endl);
                  // biomass per cell for this group specific to this landscape
                  loadedData.vectparam3.push_back(pos->second);
              }


             // if (loadedData->vectparam3.size() != (size_t) nbbenthospops) {
             //     cout << a_marine_landscape << " nb funcgr is " << loadedData->vectparam3.size() <<
             //          ": error for benthos file: the file is likely to get an extra blank space here. stop, remove and rerun."
             //          << endl;
             //     int aa;
             //     cin >> aa;
             // }


              multimap<int, double>::iterator lower_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.lower_bound(
                      a_marine_landscape);
              multimap<int, double>::iterator upper_landdd = benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.upper_bound(
                      a_marine_landscape);
              for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
                  outc(cout << pos->second << endl);
                  // logistic recovery rates for this group specific to this landscape
                  loadedData.vectparam5.push_back(pos->second);
              }


          }
      }


      multimap<int, double>::iterator lower_land3 = meanw_funcgr_per_node.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_land3 = meanw_funcgr_per_node.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_land3; pos != upper_land3; pos++) {
          outc(cout << pos->second << endl);
          // biomass per cell for this group specific to this landscape
          loadedData.vectparam2.push_back(pos->second);
      }


      multimap<int, double>::iterator lower_landd = recovery_rates_per_funcgr.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_landd = recovery_rates_per_funcgr.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_landd; pos != upper_landd; pos++) {
          outc(cout << pos->second << endl);
          // logistic recovery rates for this group specific to this landscape
          loadedData.vectparam6.push_back(pos->second);
      }

      multimap<int, double>::iterator lower_land2 = habitat_deltas_per_pop.lower_bound(a_marine_landscape);
      multimap<int, double>::iterator upper_land2 = habitat_deltas_per_pop.upper_bound(a_marine_landscape);
      for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
          outc(cout << pos->second << endl);
          // habitat_deltas_per_pop specific to this landscape
          loadedData.vectparam7.push_back(pos->second);
      }

   }

}
