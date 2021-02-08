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

#include <commons_global.h>

#include"myutils.h"
#include"Population.h"
#include "pathshop.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <options.h>
#include <comstructs.h>

namespace msqlitecpp { namespace v2 { class Storage; }}

using namespace std;

std::string getLastErrorMessage();

void open_file_error(string filename);


bool read_config_file(std::shared_ptr<msqlitecpp::v2::Storage> db, string folder_name_parameterization,
                      string inputfolder,
                      int &a_int_line2,
                      int &a_int_line4,
                      vector<int> &a_vector_line6,
                      vector<int> &implicit_pops_level2,
                      vector<int> &grouped_tacs,
                      vector<int> &nbcp_coupling_pops,
                      vector<double> &a_vector_line8,
                      vector<double> &a_vector_line10,
                      vector<double> &a_vector_line12,
                      vector<types::NodeId> &interesting_harbours);

bool read_config_file(std::istream &stream,
                      int &a_int_line2,
                      int &a_int_line4,
                      vector<int> &a_vector_line6,
                      vector<int> &implicit_pops_level2,
                      vector<int> &grouped_tacs,
                      vector<int> &nbcp_coupling_pops,
                      vector<double> &a_vector_line8,
                      vector<double> &a_vector_line10,
                      vector<double> &a_vector_line12,
                      vector<types::NodeId> &interesting_harbours);

bool read_scenario_config_file(std::shared_ptr<msqlitecpp::v2::Storage> db,
                               string folder_name_parameterization,
                               string inputfolder,
                               string namefolderoutput,
                               displace::commons::Scenario &scenario);

bool read_scenario_config_file(std::istream &stream, displace::commons::Scenario &scenario);


multimap<types::NodeId, double>
read_initial_tariffs_on_nodes(string folder_name_parameterization, string inputfolder, string a_graph_name);


// harbour specific
multimap<types::NodeId, string> read_harbour_names(string folder_name_parameterization, string inputfolder);

int read_prices_per_harbour(types::NodeId i, string a_quarter, multimap<string, double> &prices_per_harbour,
                            string folder_name_parameterization, string inputfolder);

int read_prices_per_harbour_each_pop_per_cat(types::NodeId i, string a_quarter,
                                             multimap<int, double> &prices_per_harbour_each_species_per_cat,
                                             string folder_name_parameterization, string inputfolder);

void read_fuel_prices_per_vsize(types::NodeId i, string a_quarter, map<int, double> &fuel_prices_per_vsize,
                                string folder_name_parameterization, string inputfolder);


bool read_firms_features(vector<int> &firm_ids,
                         vector<string> &firm_names,
                         vector<int> &nb_vessels,
                         vector<double> &longs,
                         vector<double> &lats,
                         string folder_name_parameterization,
                         string inputfolder);


// simu settings
vector<int> read_tsteps_quarters(string folder_name_parameterization, string inputfolder);

vector<int> read_tsteps_semesters(string folder_name_parameterization, string inputfolder);

vector<int> read_tsteps_months(string folder_name_parameterization, string inputfolder);

vector<int> read_tsteps_years(string folder_name_parameterization, string inputfolder);

//shortPaths
spp::sparse_hash_map<types::NodeId::type, types::NodeId::type>
read_maps_previous(types::NodeId source, string namesimu, string inputfolder, string a_graph_name);

spp::sparse_hash_map<types::NodeId::type, int>
read_min_distance(types::NodeId source, string namesimu, string inputfolder, string a_graph_name);

PathShop read_graph_details(types::NodeId source, string namesimu, string inputfolder, string a_graph_name);

// area based management
multimap<int, int>
read_nodes_in_polygons(string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder);

// Closure reading
struct NodeBanningInfo {
    types::NodeId nodeId;
    std::vector<int> banned;
    std::vector<double> nbOfDaysClosedPerMonth;
};

// area_closure and area_monthly_closure options
bool read_metier_quarterly_closures(vector<Node *> &nodes, string a_quarter, string a_graph,
                                    string folder_name_parameterization, string inputfolder);

bool
read_metier_monthly_closures(vector<Node *> &nodes, string a_month, string a_graph, string folder_name_parameterization,
                             string inputfolder);

bool
read_vsize_monthly_closures(vector<Node *> &nodes, string a_month, string a_graph, string folder_name_parameterization,
                            string inputfolder);

bool read_metier_closures(std::istream &stream, const string &separator, vector<NodeBanningInfo> &nodes);

bool read_vsize_closures(std::istream &stream, const string &separator, vector<NodeBanningInfo> &nodes);

bool read_biological_traits_params(istream &stream, const std::string &separator,
                                   std::vector<std::tuple<string, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, string> > &biological_traits_params);

struct EnvironmentDataRecord {
    double x;
    double y;
    int harb;
    int pt_graph;
    int code_area;
    int landscapes_code;
    double landscape_norm;
    double landscape_alpha;
    double wind, wind_norm, wind_alpha;
    double sst;
    double sst_norm;
    double sst_alpha;
    double salinity;
    double salinity_norm;
    double salinity_alpha;
    double nitrogen;
    double nitrogen_norm;
    double nitrogen_alpha;
    double phosphorus;
    double phosphorus_norm;
    double phosphorus_alpha;
    double oxygen;
    double oxygen_norm;
    double oxygen_alpha;
    double dissolvedcarbon;
    double dissolvedcarbon_norm;
    double dissolvedcarbon_alpha;
    double bathymetry;
    double shippingdensity;
    double siltfraction;
    double icesrectanglecode;
};

bool read_environment_on_coord(istream &stream, const std::string &separator,
                               std::vector<EnvironmentDataRecord> &environment_on_coord);


// SMS
void write_SMS_OP_N_in_file(ofstream &SMS_N_in, vector<Population *> &populations, vector<int> stock_numbers,
                            vector<int> a_unit, vector<int> a_number_of_ages);

void write_SMS_OP_F_in_file(ofstream &SMS_F_in, vector<Population *> &populations, vector<int> stock_numbers);

void read_SMS_OP_N_out_file(vector<Population *> &populations, vector<int> stock_numbers, vector<int> a_unit,
                            vector<int> a_number_of_ages, string sms_folder, string namesimu);


void read_pop_names_in_string(map<int, string> &pop_names, string folder_name_parameterization, string inputfolder);

multimap<int, double>
read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder);

multimap<int, double>
read_init_pops_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_init_maturity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_init_weight_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, int> read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder);

multimap<int, double> read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, types::NodeId>
read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder,
                           string str_rand_avai_file);

multimap<types::NodeId, double>
read_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder,
                                 string str_rand_avai_file, vector<string> type_of_avai_field_to_read);

multimap<types::NodeId, double>
read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization,
                                      string inputfolder, string str_rand_avai_file,
                                      vector<string> type_of_avai_field_to_read);

multimap<types::NodeId, double>
read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop, string folder_name_parameterization,
                                       string inputfolder, string biolsce);

vector<vector<double> >
read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder,
                              string biolsce);

vector<vector<double> >
read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization,
                                    string inputfolder, string biolsce);

vector<vector<double> >
read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization,
                                    string inputfolder, string biolsce);

vector<double> read_param_sr(int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);

vector<double>
read_fbar_ages_min_max_and_ftarget(int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);

vector<double> read_initial_tac(int a_pop, string folder_name_parameterization, string inputfolder);

map<int, int> read_tac_percent_simulated(string folder_name_parameterization, string inputfolder);

map<int, double> read_hyperstability_param(string folder_name_parameterization, string inputfolder);

map<types::NodeId, double>
read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization,
                             string inputfolder, string fleetsce);

int read_oth_land_map_per_met_on_nodes(map<types::NodeId, double>& oth_land, string a_semester, string a_month, int a_pop, int a_met, string folder_name_parameterization,
    string inputfolder, string fleetsce);


map<string, double>
read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder);

multimap<int, int> read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder);

multimap<int, double>
read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder,
                              string biolsce);

// pop size spectra specific
multimap<int, double>
read_adults_diet_preference_per_stock_allstks(string folder_name_parameterization, string inputfolder, string biolsce);

multimap<int, double>
read_juveniles_diet_preference_per_stock_allstks(string folder_name_parameterization, string inputfolder,
                                                 string biolsce);

// pop interaction (deprecated)
vector<vector<double> >
read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization,
                                                      string inputfolder, string biolsce);

vector<vector<double> >
read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization,
                                    string inputfolder, string biolsce);


// metier specific
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder);

multimap<int, double> read_sel_ogives(string folder_name_parameterization, string inputfolder);

multimap<int, double> read_dis_ogives(string folder_name_parameterization, string inputfolder);

multimap<int, double> read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<int, double>
read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<int, int> read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<int, int> read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder);

map<int, double> read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder);

map<int, double> read_metiers_fspeed(string folder_name_parameterization, string inputfolder);

map<int, double> read_gear_widths_param_a(string folder_name_parameterization, string inputfolder);

map<int, double> read_gear_widths_param_b(string folder_name_parameterization, string inputfolder);

map<int, string> read_gear_widths_model_type(string folder_name_parameterization, string inputfolder);

multimap<int, double>
read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization,
                                                       string inputfolder);

multimap<int, int> read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder);

multimap<int, int>
read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder);

vector<vector<double> >
read_selectivity_per_stock_ogives(int a_met, int nbpops, int nbszgroups, string folder_name_parameterization,
                                  string inputfolder, string fleetsce);

vector<vector<double> >
read_selectivity_per_stock_ogives_for_oth_land(int nbpops, int nbszgroups, string folder_name_parameterization,
                                               string inputfolder, string fleetsce);


