#ifndef DATALOADERFISHFARMS_H
#define DATALOADERFISHFARMS_H

#include<dataloader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <set>

#include "Node.h"

#include "helpers.h"
#include "options.h"

#include "msqlitecpp/v2/storage.h"


namespace sql = msqlitecpp::v2;
using namespace std;


class Node;

class [[deprecated]] Dataloaderfishfarms : public virtual Dataloader {
public:
    Dataloaderfishfarms();

    int gettype(void);

    int features(std::shared_ptr<msqlitecpp::v2::Storage> indb,
                 const string &folder_name_parameterization,
                 const string &inputfolder,
                 PopSceOptions const &dyn_pop_sce,
                 DynAllocOptions const &dyn_alloc_sce,
                 string const &biolsce,
                 string const &fleetsce,
                 ParamsForLoad &paramsForLoad,
                 LoadedData &loadedData) override;

};


bool  read_fishfarms_features(vector<int>& fishfarms_ids,
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
                             vector<double>& total_feed_kg,
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
                             string inputfolder);


map<int, double>  read_size_per_farm(string folder_name_parameterization, string inputfolder);



#endif // DATALOADERFISHFARMS_H
