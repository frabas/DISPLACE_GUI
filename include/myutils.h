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

#ifndef __MYUTILS_H
#define __MYUTILS_H (1)

#include <commons_global.h>
#include <sparsepp/spp.h>

#include <vesselcalendar.h>
#include <idtypes.h>
#include <utils/MultifieldReader.h>

#include<vector>
#include<deque>
#include<map>
#include <list>
#include <set>
#include<string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <algorithm>
#include <numeric>
#include <cmath>
#include <math.h>

#include <cstddef>
#include <pathshop.h>

#include <numeric>

using namespace std;

//---------------------------------------//
// general util.
//---------------------------------------//


template<double f(double, double, double)>
double simpson(double a, double b, int n, double S1, double S2){
    double h=(b-a)/n;
    double sum=f(a, S1, S2)*0.5;
    for (int i=1; i<n; i++) sum+=f(a+i*h, S1, S2);
    sum+= f(b, S1, S2)*0.5;

    double summid=0.0;
    for(int i=1; i<=n; i++) summid+=f(a+ (i-0.5)*h, S1, S2);

    return (sum + 2*summid)*h/3.0;
}



template <typename T>
vector<size_t> sort_indexes_ascending(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}
// usage:
//for (auto i: sort_indexes(v)) {
//  cout << v[i] << endl;
//}
template <typename T>
vector<size_t> sort_indexes_descending(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  ::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

  return idx;
}




double  trapezoidal(double a, double b, vector <double> sel);
double  myintegrand(double x, double S1, double S2);

template <typename T>
void remove_dups(vector<T>& seq)
{
    sort( seq.begin(), seq.end() ) ;
    seq.erase( unique( seq.begin(), seq.end() ), seq.end() ) ;
}

void  remove_dups(vector<int>& seq);
multimap<int,int>   remove_dups(multimap<int,int>& original_map); // keep the first pair of all keys

// to keep the first element of all the keys only:
template<typename K, typename V>
multimap<K,V>  remove_dups(multimap<K,V>& original_map)
{
    multimap<K,V> new_map;

    while (original_map.size() > 0)
    {
        auto element = *(original_map.begin());
        new_map.insert(make_pair(element.first,element.second));
        original_map.erase(element.first);
    }
    return(new_map);
}


// remove key-value duplicates
multimap<int,int>::const_iterator  find_pair(const multimap<int,int>& map, const pair<int, int>& pair);
bool  insert_if_not_present(multimap<int,int>& map, const pair<int, int>& pair);
// to remove key-value duplicates:
template <typename K, typename V>
typename multimap<K,V>::const_iterator find_pair(const multimap<K,V>& map, const pair<K, V>& pair)
{
    auto range = map.equal_range(pair.first);
    for (auto p = range.first; p != range.second; ++p)
        if (p->second == pair.second)
            return p;
    return map.end();
}

template <typename K, typename V>
bool insert_if_not_present(multimap<K,V>& map, const pair<K,V>& pair)
{
    if (find_pair(map, pair) == map.end()) {
        map.insert(pair);
        return true;
    }
    return false;
}

//void print( vector <string> & v );
//void print_d( vector <double> & v );

struct  splitX
{
    enum empties_t { empties_ok, no_empties };
};

template <typename Container>
Container& split(
        Container&                            result,
        const typename Container::value_type& s,
        const typename Container::value_type& delimiters,
        splitX::empties_t                      empties = splitX::empties_ok )
{
    result.clear();
    size_t current;
    size_t next = -1;
    do
    {
        if (empties == splitX::no_empties)
        {
            next = s.find_first_not_of( delimiters, next + 1 );
            if (next == Container::value_type::npos) break;
            next -= 1;
        }
        current = next + 1;
        next = s.find( delimiters, current );
        result.push_back( s.substr( current, next - current ) );
    }
    while (next != Container::value_type::npos);
    return result;
}




//---------------------------------------//
// graph related (Dijkstra, etc.)
//---------------------------------------//

//simple graph type definition headers>>
typedef types::NodeId::type vertex_t;
typedef int weight_t;			 // 'integer' instead of 'double' to speedup c++

struct  edge
{
    vertex_t target;
    weight_t weight;
    edge(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) { }
};

typedef std::map<vertex_t, std::list<edge> > adjacency_map_t;

void  DijkstraComputePaths(vertex_t source,
                          adjacency_map_t& adjacency_map,
                          spp::sparse_hash_map<vertex_t, weight_t>& min_distance,
                          spp::sparse_hash_map<vertex_t, vertex_t>& previous,
                          std::vector<types::NodeId> relevant_nodes);

std::list<vertex_t>   DijkstraGetShortestPathTo(vertex_t target, spp::sparse_hash_map<vertex_t, vertex_t> &previous);
std::list<types::NodeId>  DijkstraGetShortestPathTo(types::NodeId target, const PathShop &pathshop);

void  SimplifyThePreviousMap(int source,
        spp::sparse_hash_map<vertex_t, vertex_t>& previous,
        std::vector<types::NodeId> &relevant_nodes,
        spp::sparse_hash_map<vertex_t, weight_t>& min_distance,
        string namesimu,
        string a_graph_name,
        string inputfolder);

void  closeSomeNodes(std::vector<int>& nodes_to_be_closed, adjacency_map_t& adjacency_map);

vector<double>  compute_distance_fgrounds(const vector<types::NodeId> &relevant_nodes,
                                         const std::vector<PathShop> &pathshops,
                                         types::NodeId from,
                                         vector<types::NodeId> grounds);


//---------------------------------------//
// data input
//---------------------------------------//

bool  fill_from_coord (istream& in, vector<double>& graph_coord_x,
                      vector<double> & graph_coord_y,
                      vector<int>& graph_coord_harbour, int nrow);
bool  fill_from_graph (istream& in, vector<int>& graph_idx_dep,
                      vector<int> & graph_idx_arr,
                      vector<int>& graph_dist_km, int nrow);
bool  fill_from_code_area (istream& in, vector<int>& graph_point_code_area,
                          int nrow);
bool  fill_from_code_marine_landscape (istream& in, vector<int>& graph_point_code_landscape,
                                      int nrow);
bool  fill_from_wind (istream& in, vector<double>& graph_point_wind,
                                      int nrow);
bool  fill_from_sst (istream& in, vector<double>& graph_point_sst,
                                      int nrow);
bool  fill_from_salinity (istream& in, vector<double>& graph_point_salinity,
                                      int nrow);
bool  fill_from_Nitrogen (istream& in, vector<double>& graph_point_Nitrogen,
                                      int nrow);
bool  fill_from_Phosphorus (istream& in, vector<double>& graph_point_Phosphorus,
                                      int nrow);
bool  fill_from_Oxygen (istream& in, vector<double>& graph_point_Oxygen,
                                      int nrow);
bool  fill_from_DissolvedCarbon (istream& in, vector<double>& graph_point_DissolvedCarbon,
                                      int nrow);
bool  fill_from_bathymetry (istream& in, vector<double>& graph_point_bathymetry,
                                      int nrow);
bool  fill_from_shippingdensity (istream& in, vector<double>& graph_point_shippingdensity,
                                      int nrow);

bool  fill_from_siltfraction (istream& in, vector<double>& graph_point_siltfraction,
                                      int nrow);
bool  fill_from_icesrectanglecode(istream& in, vector<double>& graph_point_icesrectanglecode,
    int nrow);

bool  fill_from_benthos_biomass (istream& in, vector<double>& graph_point_benthos_biomass,
                                      int nrow);
bool  fill_from_benthos_number (istream& in, vector<double>& graph_point_benthos_number,
                                      int nrow);
bool  fill_in_growth_transition (istream& in, vector< vector<double> >& growth_transition);
bool  fill_in_species_interactions_mortality_proportion_matrix (istream& in, vector< vector<double> >& species_interactions_mortality_proportion_matrix);
bool  fill_in_preferences_for_species_matrix (istream& in, vector< vector<double> >& preferences_for_species_matrix);
bool  fill_in_selectivity_per_stock(istream& in, vector< vector<double> >& growth_transition);
bool  fill_in_percent_szgroup_per_age_matrix (istream& in, vector< vector<double> >& percent_szgroup_per_age_matrix);
bool  fill_in_percent_age_per_szgroup_matrix (istream& in, vector< vector<double> >& percent_age_per_szgroup_matrix);
bool  fill_in_param_sr (istream& in, vector<double>& param_sr);
bool  fill_in_initial_tac (istream& in, vector<double>& initial_tac);
bool  fill_in_fbar_ages_min_max (istream& in, vector<double>& fbar_ages_min_max);
bool  fill_from_metier_specifications (istream& in, multimap<string, double>& infos);
bool  fill_from_vessels_specifications(istream& in, vector<string>& names,
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
                                      vector<VesselCalendar> &calendars);

bool  fill_from_vessels_economic_specifications(istream& in,
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
                                      vector<double>& annual_discount_rates);

bool  fill_from_ships_specifications (istream& in, vector<string>& names, vector<double> &imos,
                                      vector<double> &yearbuilds, vector<string> &flags,
                                      vector<string> &types, vector<double> &typecodes,
                                      vector<double> &loas, vector<double> &KWs, vector<double> &breadths,
                                      vector<double> &grosstonnages,
                                      vector<double> &nbunits,
                                      vector<double> &fueluse,
                                      vector<double> &NOxEmission_gperKWh,
                                      vector<double> &SOxEmission_percentpertotalfuelmass,
                                      vector<double> &GHGEmission,
                                      vector<double> &PMEmission,
                                      vector<double> &vmaxs,
                                      vector<double> &vcruises,
                                      vector<double> &lane_ids);

bool read_fishfarms_features(vector<int> &fishfarms_ids,
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
);

bool fill_from_fishfarms_specifications(istream &in,
                                        vector<int> &fishfarms_ids,
                                        vector<string> &fishfarms_names,
                                        vector<int> &idx_nodes,
                                        vector<int> &is_actives,
                                        vector<double> &sizes,
                                        vector<double> &longs,
                                        vector<double> &lats,
                                        vector<double> &mean_SSTs,
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
                                    vector<int> &   nbyears_for_growths,
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
                                    vector<double>& annual_profits
                                    );
bool fill_from_firms_specifications(istream& in,
                                    vector<int> & firm_ids,
                                    vector<string> & firm_names,
                                    vector<int> & nb_vessels,
                                    vector<double> & longs,
                                    vector<double> & lats
                                    );

bool  fill_from_avai_nodes_with_pop (istream& in, map<int, double>& avai);
bool  fill_from_avai_szgroup_nodes_with_pop (istream& in, multimap<types::NodeId, double> &avai);
bool  fill_field_of_coeff_diffusion_this_pop(istream& in, multimap<types::NodeId, double> &coeffs);
bool  fill_from_oth_land (istream& in, map<types::NodeId, double> &oth_land);
bool  fill_from_overall_migration_fluxes (istream& in, multimap<int, double> &overall_migration_fluxes);
bool  fill_from_relative_stability(istream& in, map<string, double>& relative_stability);
bool  fill_from_percent_tac_per_vessel_length_class(istream& in, map<int, double>& percent_tac_per_vessel_length_class, string folder_name_parameterization);
bool  fill_from_percent_tac_cumul_over_months_key(istream& in, map<int, double>& percent_tac_cumul_over_months_key, string folder_name_parameterization);
bool  fill_from_nodes_in_polygons (istream& in, multimap<int, int>& nodes_in_polygons);

bool  fill_multimap_from_specifications_s_i(istream& in, multimap<string, int>& infos);
bool  fill_multimap_from_specifications_s_d(istream& in, multimap<string, double>& infos);


template <typename K, typename V>
bool fill_multimap_from_specifications (istream& in, multimap<K, V>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<K,V>>(in, " ", [&infos](std::tuple<K, V> v) {
                                                           infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}

bool  fill_multimap_from_specifications_i_s(istream& in, multimap<int, string>& infos);
bool  fill_multimap_from_specifications_i_d(istream& in, multimap<int, double>& infos);
bool  fill_multimap_from_specifications_i_i(istream& in, multimap<int, int>& infos);
bool  fill_map_from_specifications_i_i(istream& in, map<int, int>& a_map);
bool  fill_map_from_specifications_i_s(istream& in, map<int, string>& a_map);
bool  fill_map_from_specifications_s_d(istream& in, map<string, double>& a_map);

template <typename DTYPE>
void fill_map_from_specifications (istream &in, map<int,DTYPE> &map) {
    string line;
    while(!getline(in, line).eof())
    {
        int key;
        in >> key;
        double val;
        in >> val;
        map.insert(make_pair(key,val));
    }
}

template <typename K, typename V>
std::vector<V> find_entries(const std::multimap<K,V> &infos, K val) {
    auto upper = infos.upper_bound(val);
    vector<V> result;

    for(auto pos= infos.lower_bound(val); pos!=upper; pos++) {
        result.push_back(pos->second);
    }

    return(result);
}

vector<double>  find_entries_s_d (multimap<string, double>& infos, string str);
vector<int>  find_entries_s_i (multimap<string, int>& infos, string vid);
vector<double>  find_entries_i_d (const multimap<int, double> &infos, int intg);
vector<int>  find_entries_i_i (const multimap<int, int> &infos, int intg);
vector<string>  find_entries_i_s (multimap<int, string>& infos, int intg);
void  set_entries_d (multimap<int, double>& infos, int itr, vector<double> newval);

template <typename T, size_t N> T* end(T (&pArray)[N]);

vector<double>  scale_a_vector_to_1(vector<double> a_vector);

bool  fill_map_from_specifications_i_i(istream& in, map<int, int>& infos, string namefolderinput);
bool  fill_map_from_specifications_i_s(istream& in, map<int, string>& infos, string namefolderinput);
bool fill_map_from_specifications_i_d(istream &in, map<int, double> &infos, string namefolderinput);

// generator
//int my_rand(int a, int b)
//{
//    return a+ rand() % (b-a+1);
//}

double decode_the_tree(string &tree, vector<string> &direction,
                       map<string, int> &external_states, map<string, int> &internal_states);

bool read_ships_features(vector<string> &shipids,
                         vector<double> &imos,
                         vector<double> &yearbuilds, vector<string> &flags,
                         vector<string> &types, vector<double> &typecodes,
                         vector<double> &loas,
                         vector<double> &KWs,
                         vector<double> &breadths,
                         vector<double> &grosstonnages, vector<double> &nbunits,
                         vector<double> &fueluses,
                         vector<double> &NOxEmission_gperKWhs,
                         vector<double> &SOxEmission_percentpertotalfuelmasss,
                         vector<double> &GHGEmissions,
                         vector<double> &PMEmissions,
                         vector<double> &vmaxs,
                         vector<double> &vcruises,
                         vector<double> &lane_ids,
                         string folder_name_parameterization,
                         string inputfolder
);

multimap<int, double> read_shiplanes_lat(string folder_name_parameterization, string inputfolder);

multimap<int, double> read_shiplanes_lon(string folder_name_parameterization, string inputfolder);

map<int, double> read_size_per_windmill(string folder_name_parameterization, string inputfolder);

#endif
