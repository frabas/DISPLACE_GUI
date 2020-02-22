//
// Created by happycactus on 2/22/20.
//

#include "TextfileModelLoader.h"
#include "TextImpl/Impl.h"
#include "Fishfarm.h"
#include "SimModel.h"

#include <vector>
#include <string>


static bool read_fishfarms_features(vector<int> &fishfarms_ids,
                                    vector<string> &fishfarms_names,
                                    vector<int> &idx_nodes,
                                    vector<int> &is_actives,
                                    vector<double> &sizes,
                                    vector<double> &longs,
                                    vector<double> &lats,
                                    vector<double> &mean_SSTs,
                                    vector<double> &mean_salinities,
                                    vector<double> &mean_windspeeds,
                                    vector<double> &mean_currentspeeds,
                                    vector<double> &max_depths,
                                    vector<double> &diss_O2_mg_per_ls,
                                    vector<double> &Linf_mms,
                                    vector<double> &K_ys,
                                    vector<double> &t0_ys,
                                    vector<double> &fulton_condition_factors,
                                    vector<string> &meanw_growth_model_types,
                                    vector<int> &start_day_growings,
                                    vector<int> &end_day_harvests,
                                    vector<int> &nbyears_for_growths,
                                    vector<int> &nb_days_fallowing_periods,
                                    vector<int> &nb_fish_at_starts,
                                    vector<double> &meanw_at_starts,
                                    vector<double> &price_per_kg_at_starts,
                                    vector<double> &target_meanw_at_harvests,
                                    vector<int> &nb_fish_at_harvests,
                                    vector<double> &meanw_at_harvests,
                                    vector<double> &prop_harvest_kg_solds,
                                    vector<double> &kg_eggs_per_kgs,
                                    vector<double> &price_eggs_per_kgs,
                                    vector<double> &N_in_fish_kg_3pers,
                                    vector<double> &P_in_fish_kg_0_5pers,
                                    vector<string> &feed_types,
                                    vector<double> &feed_price_per_kgs,
                                    vector<double> &total_feed_kgs,
                                    vector<double> &prop_N_in_feeds,
                                    vector<double> &prop_P_in_feeds,
                                    vector<double> &total_feed_N_kgs,
                                    vector<double> &total_feed_P_kgs,
                                    vector<string> &feed_type_vets,
                                    vector<double> &feed_vet_price_per_kgs,
                                    vector<double> &total_feed_vet_kgs,
                                    vector<double> &prop_N_in_feed_vets,
                                    vector<double> &prop_P_in_feed_vets,
                                    vector<double> &total_feed_vet_N_kgs,
                                    vector<double> &total_feed_vet_P_kgs,
                                    vector<double> &annual_discharge_N_kgs,
                                    vector<double> &annual_discharge_P_kgs,
                                    vector<double> &annual_discharge_C_kgs,
                                    vector<double> &annual_discharge_heavymetals_kgs,
                                    vector<double> &annual_discharge_medecine_kgs,
                                    vector<double> &net_harvest_kg_per_sqkm_ys,
                                    vector<double> &market_price_sold_fishs,
                                    vector<double> &operating_cost_per_days,
                                    vector<double> &annual_profits,
                                    string folder_name_parameterization,
                                    string inputfolder
)
{


    string filename = inputfolder + "/fishfarmsspe_" + folder_name_parameterization + "/fishfarmsspe_features.dat";

    ifstream fishfarms_features;
    fishfarms_features.open(filename.c_str());
    if (fishfarms_features.fail()) {
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


using namespace std;

bool TextfileModelLoader::loadFishFarmsImpl()
{
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
                                 p->folder_name_parameterization,
                                 p->inputfolder)) {
        cerr << "Error loading fishfarms features. Bailing out.\n";
        return false;
    }

    vector<Fishfarm *> fishfarms(all_fishfarms_ids.size());
    for (unsigned int i = 0; i < all_fishfarms_ids.size(); i++) {

        cout << "create fishfarm num: " << i << endl;

        fishfarms[i] = new Fishfarm(
                all_fishfarms_ids.at(i),
                fishfarms_names.at(i),
                model().nodes().at(idx_nodes.at(i)),
                0,
                is_actives.at(i),
                fishfarms_sizes.at(i),
                fishfarms_longs.at(i),
                fishfarms_lats.at(i),
                mean_SSTs.at(i),
                mean_salinities.at(i),
                mean_windspeeds.at(i),
                mean_currentspeeds.at(i),
                max_depths.at(i),
                diss_O2_mg_per_ls.at(i),
                Linf_mms.at(i),
                K_ys.at(i),
                t0_ys.at(i),
                fulton_condition_factors.at(i),
                meanw_growth_model_types.at(i),
                start_day_growings.at(i),
                end_day_harvests.at(i),
                nbyears_for_growths.at(i),
                nb_days_fallowing_periods.at(i),
                nb_fish_at_starts.at(i),
                meanw_at_starts.at(i),
                price_per_kg_at_starts.at(i),
                target_meanw_at_harvests.at(i),
                nb_fish_at_harvests.at(i),
                meanw_at_harvests.at(i),
                prop_harvest_kg_solds.at(i),
                kg_eggs_per_kgs.at(i),
                price_eggs_per_kgs.at(i),
                N_in_fish_kg_3pers.at(i),
                P_in_fish_kg_0_5pers.at(i),
                feed_types.at(i),
                feed_price_per_kgs.at(i),
                total_feed_kgs.at(i),
                prop_N_in_feeds.at(i),
                prop_P_in_feeds.at(i),
                total_feed_N_kgs.at(i),
                total_feed_P_kgs.at(i),
                feed_type_vets.at(i),
                feed_vet_price_per_kgs.at(i),
                total_feed_vet_kgs.at(i),
                prop_N_in_feed_vets.at(i),
                prop_P_in_feed_vets.at(i),
                total_feed_vet_N_kgs.at(i),
                total_feed_vet_P_kgs.at(i),
                annual_discharge_N_kgs.at(i),
                annual_discharge_P_kgs.at(i),
                annual_discharge_C_kgs.at(i),
                annual_discharge_heavymetals_kgs.at(i),
                annual_discharge_medecine_kgs.at(i),
                net_harvest_kg_per_sqkm_ys.at(i),
                market_price_sold_fishs.at(i),
                operating_cost_per_days.at(i),
                annual_profits.at(i));


        model().nodes().at(idx_nodes.at(i))->set_ff_names_on_node(
                all_fishfarms_ids.at(i));

        cout << fishfarms[i]->get_name() << endl;
        cout << "on node " << idx_nodes.at(i) << " put ffarm "
             << model().nodes().at(idx_nodes.at(i))->get_ff_names_on_node().at(0)
             << endl;
        cout << "at (" << fishfarms[i]->get_x() << "," << fishfarms[i]->get_y() << ") " << endl;
        cout << "end for harvest at " << end_day_harvests.at(i) << " given "
             << fishfarms[i]->get_end_day_harvest()
             << endl;
    }
    model().setFishFarms(std::move(fishfarms));

    return true;
}
