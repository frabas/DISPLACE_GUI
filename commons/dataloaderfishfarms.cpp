#include "dataloaderfishfarms.h"
#include <iostream>

using namespace std;



Dataloaderfishfarms::Dataloaderfishfarms()
{

}

int Dataloaderfishfarms::gettype()
{
    return(1);
}



bool read_fishfarms_features(vector<int>& fishfarms_ids,
                         vector<string>& fishfarms_names,
                         vector<int>& idx_nodes,
                         vector<int>& is_actives,
                         vector<double>& sizes,
                         vector<double>& longs,
                         vector<double>& lats,
                         vector<double>& mean_SSTs,
                         vector<double>& mean_salinities,
                         vector<double>& mean_windspeeds,
                         vector<double>& mean_currentspeeds,
                         vector<double>& max_depths,
                         vector<double>& diss_O2_mg_per_ls,
                         vector<double>& Linf_mms,
                         vector<double>& K_ys,
                         vector<double>& t0_ys,
                         vector<double>& fulton_condition_factors,
                         vector<string>& meanw_growth_model_types,
                         vector<int>&    start_day_growings,
                         vector<int>&    end_day_harvests,
                         vector<int>&    nbyears_for_growths,
                         vector<int>&    nb_days_fallowing_periods,
                         vector<int>&    nb_fish_at_starts,
                         vector<double>& meanw_at_starts,
                         vector<double>& price_per_kg_at_starts,
                         vector<double>& target_meanw_at_harvests,
                         vector<int>& nb_fish_at_harvests,
                         vector<double>& meanw_at_harvests,
                         vector<double>& prop_harvest_kg_solds,
                         vector<double>& kg_eggs_per_kgs,
                         vector<double>& price_eggs_per_kgs,
                         vector<double>& N_in_fish_kg_3pers,
                         vector<double>& P_in_fish_kg_0_5pers,
                         vector<string>& feed_types,
                         vector<double>& feed_price_per_kgs,
                         vector<double>& total_feed_kgs,
                         vector<double>& prop_N_in_feeds,
                         vector<double>& prop_P_in_feeds,
                         vector<double>& total_feed_N_kgs,
                         vector<double>& total_feed_P_kgs,
                         vector<string>& feed_type_vets,
                         vector<double>& feed_vet_price_per_kgs,
                         vector<double>& total_feed_vet_kgs,
                         vector<double>& prop_N_in_feed_vets,
                         vector<double>& prop_P_in_feed_vets,
                         vector<double>& total_feed_vet_N_kgs,
                         vector<double>& total_feed_vet_P_kgs,
                         vector<double>& annual_discharge_N_kgs,
                         vector<double>& annual_discharge_P_kgs,
                         vector<double>& annual_discharge_C_kgs,
                         vector<double>& annual_discharge_heavymetals_kgs,
                         vector<double>& annual_discharge_medecine_kgs,
                         vector<double>& net_harvest_kg_per_sqkm_ys,
                         vector<double>& market_price_sold_fishs,
                         vector<double>& operating_cost_per_days,
                         vector<double>& annual_profits,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{


    string filename=  inputfolder+"/fishfarmsspe_"+folder_name_parameterization+"/fishfarmsspe_features.dat";

    ifstream fishfarms_features;
    fishfarms_features.open(filename.c_str());
    if(fishfarms_features.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }



    bool r = fill_from_fishfarms_specifications(fishfarms_features,
                                                fishfarms_ids,
                                                fishfarms_names,
                                                idx_nodes,
                                                is_actives,
                                                sizes,
                                                longs,
                                                lats,
                                                mean_SSTs,
                                                mean_salinities,
                                                mean_windspeeds,
                                                mean_currentspeeds,
                                                max_depths,
                                                diss_O2_mg_per_ls,
                                                Linf_mms,
                                                K_ys,
                                                t0_ys,
                                                fulton_condition_factors,
                                                meanw_growth_model_types,
                                                start_day_growings,
                                                end_day_harvests,
                                                nbyears_for_growths,
                                                nb_days_fallowing_periods,
                                                nb_fish_at_starts,
                                                meanw_at_starts,
                                                price_per_kg_at_starts,
                                                target_meanw_at_harvests,
                                                nb_fish_at_harvests,
                                                meanw_at_harvests,
                                                prop_harvest_kg_solds,
                                                kg_eggs_per_kgs,
                                                price_eggs_per_kgs,
                                                N_in_fish_kg_3pers,
                                                P_in_fish_kg_0_5pers,
                                                feed_types,
                                                feed_price_per_kgs,
                                                total_feed_kgs,
                                                prop_N_in_feeds,
                                                prop_P_in_feeds,
                                                total_feed_N_kgs,
                                                total_feed_P_kgs,
                                                feed_type_vets,
                                                feed_vet_price_per_kgs,
                                                total_feed_vet_kgs,
                                                prop_N_in_feed_vets,
                                                prop_P_in_feed_vets,
                                                total_feed_vet_N_kgs,
                                                total_feed_vet_P_kgs,
                                                annual_discharge_N_kgs,
                                                annual_discharge_P_kgs,
                                                annual_discharge_C_kgs,
                                                annual_discharge_heavymetals_kgs,
                                                annual_discharge_medecine_kgs,
                                                net_harvest_kg_per_sqkm_ys,
                                                market_price_sold_fishs,
                                                operating_cost_per_days,
                                                annual_profits
                                                );
    fishfarms_features.close();

    return r;
}





// FOR FISHFARMS
map<int, double> read_size_per_farm(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/fishfarmsspe_"+folder_name_parameterization+"/size_per_farm.dat";

    ifstream file_size_per_farm;
    file_size_per_farm.open(filename.c_str());
    if(file_size_per_farm.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    map<int, double> size_per_farm;
    fill_map_from_specifications(file_size_per_farm,  size_per_farm);
    file_size_per_farm.close();

    return(size_per_farm);
}



int Dataloaderfishfarms::features(std::shared_ptr<sql::Storage> indb,
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
  cout << "Loading fishfarms features" << endl;


  // read general firm features
  vector<int> all_fishfarms_ids;
  vector<string> fishfarms_names;
  vector<int> idx_nodes;
  vector<int> is_actives;
  vector<double> fishfarms_sizes;
  vector<double> fishfarms_longs;
  vector<double> fishfarms_lats;
  vector<double> mean_SSTs;
  vector<double> mean_salinities;
  vector<double> mean_windspeeds;
  vector<double> mean_currentspeeds;
  vector<double> max_depths;
  vector<double> diss_O2_mg_per_ls;
  vector<double> Linf_mms;
  vector<double> K_ys;
  vector<double> t0_ys;
  vector<double> fulton_condition_factors;
  vector<string> meanw_growth_model_types;
  vector<int> start_day_growings;
  vector<int> end_day_harvests;
  vector<int> nbyears_for_growths;
  vector<int> nb_days_fallowing_periods;
  vector<int> nb_fish_at_starts;
  vector<double> meanw_at_starts;
  vector<double> price_per_kg_at_starts;
  vector<double> target_meanw_at_harvests;
  vector<int> nb_fish_at_harvests;
  vector<double> meanw_at_harvests;
  vector<double> prop_harvest_kg_solds;
  vector<double> kg_eggs_per_kgs;
  vector<double> price_eggs_per_kgs;
  vector<double> N_in_fish_kg_3pers;
  vector<double> P_in_fish_kg_0_5pers;
  vector<string> feed_types;
  vector<double> feed_price_per_kgs;
  vector<double> total_feed_kgs;
  vector<double> prop_N_in_feeds;
  vector<double> prop_P_in_feeds;
  vector<double> total_feed_N_kgs;
  vector<double> total_feed_P_kgs;
  vector<string> feed_type_vets;
  vector<double> feed_vet_price_per_kgs;
  vector<double> total_feed_vet_kgs;
  vector<double> prop_N_in_feed_vets;
  vector<double> prop_P_in_feed_vets;
  vector<double> total_feed_vet_N_kgs;
  vector<double> total_feed_vet_P_kgs;
  vector<double> annual_discharge_N_kgs;
  vector<double> annual_discharge_P_kgs;
  vector<double> annual_discharge_C_kgs;
  vector<double> annual_discharge_heavymetals_kgs;
  vector<double> annual_discharge_medecine_kgs;
  vector<double> net_harvest_kg_per_sqkm_ys;
  vector<double> market_price_sold_fishs;
  vector<double> operating_cost_per_days;
  vector<double> annual_profits;


  if (!read_fishfarms_features(all_fishfarms_ids, fishfarms_names, idx_nodes, is_actives, fishfarms_sizes,
                               fishfarms_longs, fishfarms_lats,
                               mean_SSTs,
                               mean_salinities,
                               mean_windspeeds,
                               mean_currentspeeds,
                               max_depths,
                               diss_O2_mg_per_ls,
                               Linf_mms,
                               K_ys,
                               t0_ys,
                               fulton_condition_factors,
                               meanw_growth_model_types,
                               start_day_growings,
                               end_day_harvests,
                               nbyears_for_growths,
                               nb_days_fallowing_periods,
                               nb_fish_at_starts,
                               meanw_at_starts,
                               price_per_kg_at_starts,
                               target_meanw_at_harvests,
                               nb_fish_at_harvests,
                               meanw_at_harvests,
                               prop_harvest_kg_solds,
                               kg_eggs_per_kgs,
                               price_eggs_per_kgs,
                               N_in_fish_kg_3pers,
                               P_in_fish_kg_0_5pers,
                               feed_types,
                               feed_price_per_kgs,
                               total_feed_kgs,
                               prop_N_in_feeds,
                               prop_P_in_feeds,
                               total_feed_N_kgs,
                               total_feed_P_kgs,
                               feed_type_vets,
                               feed_vet_price_per_kgs,
                               total_feed_vet_kgs,
                               prop_N_in_feed_vets,
                               prop_P_in_feed_vets,
                               total_feed_vet_N_kgs,
                               total_feed_vet_P_kgs,
                               annual_discharge_N_kgs,
                               annual_discharge_P_kgs,
                               annual_discharge_C_kgs,
                               annual_discharge_heavymetals_kgs,
                               annual_discharge_medecine_kgs,
                               net_harvest_kg_per_sqkm_ys,
                               market_price_sold_fishs,
                               operating_cost_per_days,
                               annual_profits,
                               folder_name_parameterization, inputfolder)) {
      cerr << "Error loading fishfarms features. Bailing out.\n";
      return 2;
  }




  //  export
  loadedData.vectiparam1=all_fishfarms_ids;
  loadedData.vectsparam1=fishfarms_names;
  loadedData.vectiparam2=idx_nodes;
  loadedData.vectiparam3=is_actives;
  loadedData.vectdparam1=fishfarms_sizes;
  loadedData.vectdparam2=fishfarms_longs;
  loadedData.vectdparam3=fishfarms_lats;
  loadedData.vectdparam4=mean_SSTs;
  loadedData.vectdparam5=mean_salinities;
  loadedData.vectdparam6=mean_windspeeds;
  loadedData.vectdparam7=mean_currentspeeds;
  loadedData.vectdparam8=max_depths;
  loadedData.vectdparam9=diss_O2_mg_per_ls;
  loadedData.vectdparam10=Linf_mms;
  loadedData.vectdparam11=K_ys;
  loadedData.vectdparam12=t0_ys;
  loadedData.vectdparam13=fulton_condition_factors;
  loadedData.vectsparam2=meanw_growth_model_types;
  loadedData.vectiparam4=start_day_growings;
  loadedData.vectiparam5=end_day_harvests;
  loadedData.vectiparam6=nbyears_for_growths;
  loadedData.vectiparam7=nb_days_fallowing_periods;
  loadedData.vectiparam8=nb_fish_at_starts;
  loadedData.vectdparam14=meanw_at_starts;
  loadedData.vectdparam15=price_per_kg_at_starts;
  loadedData.vectdparam16=target_meanw_at_harvests;
  loadedData.vectiparam9=nb_fish_at_harvests;
  loadedData.vectdparam17=meanw_at_harvests;
  loadedData.vectdparam18=prop_harvest_kg_solds;
  loadedData.vectdparam19=kg_eggs_per_kgs;
  loadedData.vectdparam20=price_eggs_per_kgs;
  loadedData.vectdparam21=N_in_fish_kg_3pers;
  loadedData.vectdparam22=P_in_fish_kg_0_5pers;
  loadedData.vectsparam3=feed_types;
  loadedData.vectdparam23=feed_price_per_kgs;
  loadedData.vectdparam24=total_feed_kgs;
  loadedData.vectdparam25=prop_N_in_feeds;
  loadedData.vectdparam26=prop_P_in_feeds;
  loadedData.vectdparam27=total_feed_N_kgs;
  loadedData.vectdparam28=total_feed_P_kgs;
  loadedData.vectsparam4=feed_type_vets;
  loadedData.vectdparam29=feed_vet_price_per_kgs;
  loadedData.vectdparam30=total_feed_vet_kgs;
  loadedData.vectdparam31=prop_N_in_feed_vets;
  loadedData.vectdparam32=prop_P_in_feed_vets;
  loadedData.vectdparam33=total_feed_vet_N_kgs;
  loadedData.vectdparam34=total_feed_vet_P_kgs;
  loadedData.vectdparam35=annual_discharge_N_kgs;
  loadedData.vectdparam36=annual_discharge_P_kgs;
  loadedData.vectdparam37=annual_discharge_C_kgs;
  loadedData.vectdparam38=annual_discharge_heavymetals_kgs;
  loadedData.vectdparam39=annual_discharge_medecine_kgs;
  loadedData.vectdparam40=net_harvest_kg_per_sqkm_ys;
  loadedData.vectdparam41=market_price_sold_fishs;
  loadedData.vectdparam42=operating_cost_per_days;
  loadedData.vectdparam43=annual_profits;







return 0;
}

