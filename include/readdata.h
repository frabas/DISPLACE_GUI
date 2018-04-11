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

#include <commons_global.h>

#include"myutils.h"
#include"Population.h"
#include "pathshop.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <options.h>
#include <comstructs.h>

using namespace std;

std::string COMMONSSHARED_EXPORT getLastErrorMessage();
void COMMONSSHARED_EXPORT open_file_error(string filename);

bool COMMONSSHARED_EXPORT read_config_file (string folder_name_parameterization,
string inputfolder,
int& a_int_line2,
int& a_int_line4,
vector<int>& a_vector_line6,
vector<int> &implicit_pops_level2,
vector<int> &grouped_tacs,
vector<double>& a_vector_line8,
vector<double>& a_vector_line10,
vector<double>& a_vector_line12,
vector<types::NodeId> &interesting_harbours);

bool COMMONSSHARED_EXPORT read_config_file (std::istream &stream,
int& a_int_line2,
int& a_int_line4,
vector<int>& a_vector_line6,
vector<int> &implicit_pops_level2,
vector<int> &grouped_tacs,
vector<double>& a_vector_line8,
vector<double>& a_vector_line10,
vector<double>& a_vector_line12,
vector<types::NodeId> &interesting_harbours);

bool COMMONSSHARED_EXPORT read_scenario_config_file(string folder_name_parameterization,
    string inputfolder,
    string namefolderoutput,
    displace::commons::Scenario &scenario);

bool COMMONSSHARED_EXPORT read_scenario_config_file(std::istream &stream, displace::commons::Scenario &scenario);

// vessel specific
bool COMMONSSHARED_EXPORT read_vessels_features(string a_quarter,
                           vector<string>& vesselids,
                           vector<int>& vid_is_actives,
                           vector<int>& vid_is_part_of_ref_fleets,
                           vector<double>& speeds,
                           vector<double>& fuelcons,
                           vector<double>& lengths,
                           vector<double>& vKWs,
                           vector<double>& carrycapacities,
                           vector<double>& tankcapacities,
                           vector<double>& nbfpingspertrips,
                           vector<double>& resttime_par1s,
                           vector<double>& resttime_par2s,
                           vector<double>& av_trip_duration,
                           vector<double>& mult_fuelcons_when_steaming,
                           vector<double>& mult_fuelcons_when_fishing,
                           vector<double>& mult_fuelcons_when_returning,
                           vector<double>& mult_fuelcons_when_inactive,
                           vector<int>& firm_ids,
                           string folder_name_parameterization,
                           string inputfolder,
                           int selected_vessels_only, vector<VesselCalendar> &calendars);

bool COMMONSSHARED_EXPORT read_vessels_economics_features(vector<string>& vesselids,
                           vector<double>& this_vessel_nb_crews,
                           vector<double>& annual_other_incomes,
                           vector<double>& landing_costs_percents,
                           vector<double>& crewshare_and_unpaid_labour_costs_percents,
                           vector<double>& other_variable_costs_per_unit_efforts,
                           vector<double>& annual_insurance_costs_per_crews,
                           vector<double>& standard_labour_hour_opportunity_costss,
                           vector<double>& standard_annual_full_time_employement_hourss,
                           vector<double>& other_annual_fixed_costss,
                           vector<double>& vessel_values,
                           vector<double>& annual_depreciation_rates,
                           vector<double>& opportunity_interest_rates,
                           vector<double>& annual_discount_rates,
                           string folder_name_parameterization,
                           string inputfolder
                           );


bool COMMONSSHARED_EXPORT read_ships_features(vector<string>& shipids, vector<double> &imos,
                         vector<double> &yearbuilds, vector<string> &flags,
                         vector<string> &types, vector<double> &typecodes,
                         vector<double> &loas, vector<double> &KWs, vector<double> &breadths,
                         vector<double> &grosstonnages, vector<double> &nbunits,
                         vector<double> &fueluses,
                         vector<double> &NOxEmission_gperKWhs,
                         vector<double> &SOxEmission_percentpertotalfuelmasss,
                         vector<double> &GHGEmissions,
                         vector<double> &PMEmissions,
                         vector<double>& vmaxs, vector<double>& vcruises, vector<double>& lane_ids,
                         string folder_name_parameterization, string inputfolder);

bool COMMONSSHARED_EXPORT read_fishfarms_features(vector<int>& fishfarms_ids,
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
bool COMMONSSHARED_EXPORT read_firms_features(vector<int>& firm_ids,
                         vector<string>& firm_names,
                         vector<int>& nb_vessels,
                         vector<double>& longs,
                         vector<double>& lats,
                         string folder_name_parameterization,
                         string inputfolder);




multimap<string, types::NodeId> COMMONSSHARED_EXPORT read_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, types::NodeId> COMMONSSHARED_EXPORT read_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, types::NodeId> COMMONSSHARED_EXPORT read_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> COMMONSSHARED_EXPORT read_freq_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> COMMONSSHARED_EXPORT read_freq_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> COMMONSSHARED_EXPORT read_freq_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> COMMONSSHARED_EXPORT read_vessels_betas(string a_semester, string folder_name_parameterization, string inputfolder);
multimap<string, double> COMMONSSHARED_EXPORT read_vessels_tacs(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<string, double> COMMONSSHARED_EXPORT read_initial_fishing_credits(string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, int> COMMONSSHARED_EXPORT read_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_freq_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_gshape_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_initial_tariffs_on_nodes(string folder_name_parameterization, string inputfolder, string a_graph_name);



// harbour specific
multimap<types::NodeId, string> COMMONSSHARED_EXPORT read_harbour_names(string folder_name_parameterization, string inputfolder);
int COMMONSSHARED_EXPORT read_prices_per_harbour(types::NodeId i, string a_quarter, multimap<string, double>& prices_per_harbour, string folder_name_parameterization, string inputfolder);
int COMMONSSHARED_EXPORT read_prices_per_harbour_each_pop_per_cat(types::NodeId i, string a_quarter, multimap<int, double>& prices_per_harbour_each_species_per_cat, string folder_name_parameterization, string inputfolder);
void COMMONSSHARED_EXPORT read_fuel_prices_per_vsize(map<int, double> &fuel_prices_per_vsize, string folder_name_parameterization, string inputfolder);

// metier specific
map<int, int> COMMONSSHARED_EXPORT read_metiers_types(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_sel_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_dis_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, double> COMMONSSHARED_EXPORT read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, int> COMMONSSHARED_EXPORT read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, int> COMMONSSHARED_EXPORT read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder) ;
map<int, double> COMMONSSHARED_EXPORT read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder);
map<int, double> COMMONSSHARED_EXPORT read_metiers_fspeed(string folder_name_parameterization, string inputfolder);
map<int, double> COMMONSSHARED_EXPORT read_gear_widths_param_a(string folder_name_parameterization, string inputfolder);
map<int, double> COMMONSSHARED_EXPORT read_gear_widths_param_b(string folder_name_parameterization, string inputfolder);
map<int, string> COMMONSSHARED_EXPORT read_gear_widths_model_type(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_logistic_recovery_rates_per_month_per_funcgr(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_habitat_deltas_per_pop(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_benthos_number_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder);
multimap<int, int> COMMONSSHARED_EXPORT read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder);
multimap<int, int> COMMONSSHARED_EXPORT read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder);
vector< vector<double> > COMMONSSHARED_EXPORT read_selectivity_per_stock_ogives(int a_met, int nbpops, int nbszgroups, string folder_name_parameterization, string inputfolder, string fleetsce);

// benthos specific
multimap<int, double> COMMONSSHARED_EXPORT read_prop_funcgr_biomass_per_node_per_landscape(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_prop_funcgr_number_per_node_per_landscape(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_meanw_funcgr_per_landscape(string folder_name_parameterization, string inputfolder);

// fishfarms specific
map<int, double> COMMONSSHARED_EXPORT read_size_per_farm(string folder_name_parameterization, string inputfolder);

// windmills specific
map<int, double> COMMONSSHARED_EXPORT read_size_per_windmill(string folder_name_parameterization, string inputfolder);

// shipping specific
multimap<int, double> COMMONSSHARED_EXPORT read_shiplanes_lon(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_shiplanes_lat(string folder_name_parameterization, string inputfolder);

// pop specific
// beta per pop fro a given szgroup
multimap<int, double> COMMONSSHARED_EXPORT read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_init_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_maturity_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_weight_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, int> COMMONSSHARED_EXPORT read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, types::NodeId> COMMONSSHARED_EXPORT read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder, string str_rand_avai_file);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file, string type_of_avai_field_to_read);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file, string type_of_avai_field_to_read);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> COMMONSSHARED_EXPORT read_param_sr(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> COMMONSSHARED_EXPORT read_fbar_ages_min_max_and_ftarget(int a_pop,  string folder_name_parameterization, string inputfolder);
vector<double> COMMONSSHARED_EXPORT read_initial_tac(int a_pop,  string folder_name_parameterization, string inputfolder);
map<int, int> COMMONSSHARED_EXPORT read_tac_percent_simulated(string folder_name_parameterization, string inputfolder);
map<int, double> COMMONSSHARED_EXPORT read_hyperstability_param(string folder_name_parameterization, string inputfolder);
map<types::NodeId, double> COMMONSSHARED_EXPORT read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization, string inputfolder, string fleetsce);
map<string, double> COMMONSSHARED_EXPORT read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder);
multimap<int, int> COMMONSSHARED_EXPORT read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);

// simu settings
vector <int> COMMONSSHARED_EXPORT read_tsteps_quarters(string folder_name_parameterization, string inputfolder);
vector <int> COMMONSSHARED_EXPORT read_tsteps_semesters(string folder_name_parameterization, string inputfolder);
vector <int> COMMONSSHARED_EXPORT read_tsteps_months(string folder_name_parameterization, string inputfolder);
vector <int> COMMONSSHARED_EXPORT read_tsteps_years(string folder_name_parameterization, string inputfolder);

//shortPaths
spp::sparse_hash_map<types::NodeId::type, types::NodeId::type> COMMONSSHARED_EXPORT read_maps_previous(types::NodeId source, string namesimu, string inputfolder, string a_graph_name);
spp::sparse_hash_map<types::NodeId::type, int> COMMONSSHARED_EXPORT read_min_distance(types::NodeId source, string namesimu, string inputfolder, string a_graph_name);
PathShop COMMONSSHARED_EXPORT read_graph_details(types::NodeId source, string namesimu,  string inputfolder, string a_graph_name);

// area based management
multimap<int, int> COMMONSSHARED_EXPORT read_nodes_in_polygons(string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder);

// Closure reading
struct COMMONSSHARED_EXPORT NodeBanningInfo {
    types::NodeId nodeId;
    std::vector<int> banned;
};

// area_closure and area_monthly_closure options
bool COMMONSSHARED_EXPORT read_metier_quarterly_closures (vector <Node*> &nodes, string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder);
bool COMMONSSHARED_EXPORT read_metier_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder);
bool COMMONSSHARED_EXPORT read_vsize_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder);
bool COMMONSSHARED_EXPORT read_metier_closures (std::istream &stream, const string &separator, vector <NodeBanningInfo> &nodes, double& nbOfDaysClosedPerMonth);
bool COMMONSSHARED_EXPORT read_vsize_closures (std::istream &stream, const string &separator, vector <NodeBanningInfo> &nodes, double& nbOfDaysClosedPerMonth);

bool COMMONSSHARED_EXPORT read_biological_traits_params(istream &stream, const std::string &separator, vector <std::tuple< string, double, double, double, double,
                                                        double, double, double, double,
                                                        double, double, double, double,
                                                        double, double, double, double,
                                                        double, double, double, double,
                                                        double, double, double> > & biological_traits_params);
bool COMMONSSHARED_EXPORT read_environment_on_coord(istream &stream, const std::string &separator,   std::vector <std::tuple< double, double, int,
                                                    int, int,
                                                    int, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double, double, double,
                                                    double> > & environment_on_coord);



// SMS
void COMMONSSHARED_EXPORT write_SMS_OP_N_in_file(ofstream& SMS_N_in, vector<Population* >& populations, vector<int> stock_numbers, vector<int> a_unit, vector<int> a_number_of_ages);
void COMMONSSHARED_EXPORT write_SMS_OP_F_in_file(ofstream& SMS_F_in, vector<Population* >& populations, vector<int> stock_numbers);
void COMMONSSHARED_EXPORT read_SMS_OP_N_out_file(vector<Population* >& populations, vector<int> stock_numbers, vector<int> a_unit, vector<int> a_number_of_ages, string sms_folder, string namesimu);

