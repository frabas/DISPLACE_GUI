// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>

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

#include"myutils.h"
#include"Population.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

std::string getLastErrorMessage();
void open_file_error(string filename);

void read_config_file (string folder_name_parameterization,
string inputfolder,
int& a_int_line2,
vector<int>& a_vector_line4,
vector<double>& a_vector_line6,
vector<double>& a_vector_line8,
vector<double>& a_vector_line10
);

int read_scenario_config_file(
    string folder_name_parameterization,
    string inputfolder,
    string namefolderoutput,
    vector<string>& dyn_alloc_sce,
    vector<string>& dyn_pop_sce,
    string& biolsce,
    int& a_graph,
    int& nrow_coord,
    int& nrow_graph,
    int& a_port,
    double& graph_res );

// vessel specific
void read_vessels_features(string a_quarter, vector<string>& vesselids, vector<double>& speeds,
vector<double>& fuelcons, vector<double>& lengths, vector<double>& KWs,
vector<double>& carrycapacities, vector<double>& tankcapacities,
vector<double>& nbfpingspertrips,
vector<double>& resttime_par1s, vector<double>& resttime_par2s,
vector<double>& av_trip_duration,
string folder_name_parameterization,
string inputfolder,
int selected_vessels_only);
void read_ships_features(  vector<string>& shipids,
vector<double>& vmaxs,
vector<double>& vcruises,
vector<double>& lane_ids,
string folder_name_parameterization,
string inputfolder
);

multimap<string, int> read_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, int> read_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> read_freq_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> read_freq_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);
multimap<string, double> read_vessels_betas(string a_semester, string folder_name_parameterization, string inputfolder);
multimap<string, double> read_vessels_tacs(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<int, int> read_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<int, double> read_freq_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<int, double> read_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<int, double> read_gshape_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);
multimap<int, double> read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

// harbour specific
multimap<int, string> read_harbour_names(string folder_name_parameterization, string inputfolder);
int read_prices_per_harbour(int i, string a_quarter, multimap<string, double>& prices_per_harbour, string folder_name_parameterization, string inputfolder);
int read_prices_per_harbour_each_pop_per_cat(int i, string a_quarter, multimap<int, double>& prices_per_harbour_each_species_per_cat, string folder_name_parameterization, string inputfolder);
void read_fuel_prices_per_vsize(map<string, double>& fuel_prices_per_vsize, string folder_name_parameterization, string inputfolder);

// metier specific
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder);
multimap<int, double> read_sel_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double> read_dis_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double> read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder) ;
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder);
map<int, double> read_gear_widths_param_a(string folder_name_parameterization, string inputfolder);
map<int, double> read_gear_widths_param_b(string folder_name_parameterization, string inputfolder);
map<int, string> read_gear_widths_model_type(string folder_name_parameterization, string inputfolder);
multimap<int, double> read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization, string inputfolder);

// benthos specific
multimap<int, double> read_estimates_biomass_per_cell_per_funcgr_per_landscape(string folder_name_parameterization, string inputfolder);

// pop specific
								 // beta per pop fro a given szgroup
multimap<int, double> read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder);
multimap<int, double> read_init_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> read_init_maturity_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> read_init_weight_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, int> read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder);
multimap<int, double> read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, int> read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder, string str_rand_avai_file);
multimap<int, double> read_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file);
multimap<int, double> read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file);
vector< vector<double> > read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> read_param_sr(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> read_fbar_ages_min_max_and_ftarget(int a_pop,  string folder_name_parameterization, string inputfolder);
vector<double> read_initial_tac(int a_pop,  string folder_name_parameterization, string inputfolder);
map<int, int> read_tac_percent_simulated(string folder_name_parameterization, string inputfolder);
map<int, double> read_oth_land_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder);
map<string, double> read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder);
multimap<int, int> read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder);

// simu settings
vector <int> read_tsteps_quarters(string folder_name_parameterization, string inputfolder);
vector <int> read_tsteps_semesters(string folder_name_parameterization, string inputfolder);
vector <int> read_tsteps_months(string folder_name_parameterization, string inputfolder);
vector <int> read_tsteps_years(string folder_name_parameterization, string inputfolder);

//shortPaths
map<int, int> read_maps_previous(int source, string namesimu, string inputfolder, string a_graph_name);
map<int, int> read_min_distance(int source, string namesimu, string inputfolder, string a_graph_name);

// area based management
multimap<int, int> read_nodes_in_polygons(string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder);

// SMS
void write_SMS_OP_N_in_file(ofstream& SMS_N_in, vector<Population* >& populations, vector<int> stock_numbers, vector<int> a_unit, vector<int> a_number_of_ages);
void write_SMS_OP_F_in_file(ofstream& SMS_F_in, vector<Population* >& populations, vector<int> stock_numbers);
void read_SMS_OP_N_out_file(vector<Population* >& populations, vector<int> stock_numbers, vector<int> a_unit, vector<int> a_number_of_ages);
