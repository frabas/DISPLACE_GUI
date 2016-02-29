// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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
#include <cmath>
#include <math.h>

#include <cstddef>

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



double trapezoidal(double a, double b, vector <double> sel);
double myintegrand(double x, double S1, double S2);


void remove_dups(vector<int>& seq);
multimap<int,int>  remove_dups(multimap<int,int>& original_map); // keep the first pair of all keys

// remove key-value duplicates
multimap<int,int>::const_iterator find_pair(const multimap<int,int>& map, const pair<int, int>& pair);
bool insert_if_not_present(multimap<int,int>& map, const pair<int, int>& pair);


//void print( vector <string> & v );
//void print_d( vector <double> & v );

struct splitX
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
typedef int vertex_t;
typedef int weight_t;			 // 'integer' instead of 'double' to speedup c++

struct edge
{
	vertex_t target;
	weight_t weight;
	edge(vertex_t arg_target, weight_t arg_weight)
		: target(arg_target), weight(arg_weight) { }
};

typedef std::map<vertex_t, std::list<edge> > adjacency_map_t;

void DijkstraComputePaths(vertex_t source,
adjacency_map_t& adjacency_map,
std::map<vertex_t, weight_t>& min_distance,
std::map<vertex_t, vertex_t>& previous,
std::vector<int> relevant_nodes);

std::list<vertex_t> DijkstraGetShortestPathTo(
vertex_t target, std::map<vertex_t, vertex_t>& previous);

void SimplifyThePreviousMap(
int source,
std::map<vertex_t, vertex_t>& previous,
std::vector<int>& relevant_nodes,
std::map<vertex_t, weight_t>& min_distance,
string namesimu,
string a_graph_name,
string inputfolder);

void closeSomeNodes(std::vector<int>& nodes_to_be_closed, adjacency_map_t& adjacency_map);

vector<double> compute_distance_fgrounds(const vector<int> &idx_path_shop,
const deque<map<vertex_t, vertex_t> > &path_shop,
const deque<map<vertex_t, weight_t> > &min_distance_shop,
int from,
vector<int> grounds);

//---------------------------------------//
// data input
//---------------------------------------//

void fill_from_coord (istream& in, vector<double>& graph_coord_x,
vector<double> & graph_coord_y,
vector<int>& graph_coord_harbour, int nrow);
void fill_from_graph (istream& in, vector<int>& graph_idx_dep,
vector<int> & graph_idx_arr,
vector<int>& graph_dist_km, int nrow);
void fill_from_code_area (istream& in, vector<int>& graph_point_code_area,
int nrow);
void fill_from_code_marine_landscape (istream& in, vector<int>& graph_point_code_landscape,
int nrow);
void fill_in_growth_transition (istream& in, vector< vector<double> >& growth_transition);
void fill_in_percent_szgroup_per_age_matrix (istream& in, vector< vector<double> >& percent_szgroup_per_age_matrix);
void fill_in_percent_age_per_szgroup_matrix (istream& in, vector< vector<double> >& percent_age_per_szgroup_matrix);
void fill_in_param_sr (istream& in, vector<double>& param_sr);
void fill_in_initial_tac (istream& in, vector<double>& initial_tac);
void fill_in_fbar_ages_min_max (istream& in, vector<double>& fbar_ages_min_max);
void fill_from_metier_specifications (istream& in, multimap<string, double>& infos);
void fill_from_vessels_specifications (istream& in, vector<string>& names,
vector<double>& speeds, vector<double>& fuelcons, vector<double>& lengths,  vector<double>& KWs,
vector<double>& carrycapacities, vector<double>& tankcapacities,
vector<double>& nbfpingspertrips,
vector<double>& resttime_par1s,vector<double>& resttime_par2s,
vector<double>& av_trip_duration,
                                       vector<double>& mult_fuelcons_when_steaming,
                                       vector<double>& mult_fuelcons_when_fishing,
                                       vector<double>& mult_fuelcons_when_returning,
                                       vector<double>& mult_fuelcons_when_inactive);
void fill_from_ships_specifications (istream& in, vector<string>& names, vector<double> &imos,
                                     vector<double> &yearbuilds, vector<string> &flags,
                                     vector<string> &types, vector<double> &typecodes,
                                     vector<double> &loas, vector<double> &breadths,
                                     vector<double> &grosstonnages, vector<double> &nbunits,
                                     vector<double>& vmaxs,
                                     vector<double>& vcruises,
                                     vector<double>& lane_ids);

void fill_from_avai_nodes_with_pop (istream& in, map<int, double>& avai);
void fill_from_avai_szgroup_nodes_with_pop (istream& in, multimap<int, double>& avai);
void fill_from_oth_land (istream& in, map<int, double>& oth_land);
void fill_from_overall_migration_fluxes (istream& in, multimap<int, double> &overall_migration_fluxes);
void fill_from_relative_stability (istream& in, map<string, double>& relative_stability);
void fill_from_nodes_in_polygons (istream& in, multimap<int, int>& nodes_in_polygons);

void fill_multimap_from_specifications_s_i (istream& in, multimap<string, int>& infos);
void fill_multimap_from_specifications_s_d (istream& in, multimap<string, double>& infos);
void fill_multimap_from_specifications_i_s (istream& in, multimap<int, string>& infos);
void fill_multimap_from_specifications_i_d (istream& in, multimap<int, double>& infos);
void fill_multimap_from_specifications_i_i (istream& in, multimap<int, int>& infos);
void fill_map_from_specifications_i_i (istream& in, map<int, int>& a_map);
void fill_map_from_specifications_i_s (istream& in, map<int, string>& a_map);
void fill_map_from_specifications_s_d (istream& in, map<string, double>& a_map);

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

vector<double> find_entries_s_d (multimap<string, double>& infos, string str);
vector<int> find_entries_s_i (multimap<string, int>& infos, string vid);
vector<double> find_entries_i_d (const multimap<int, double> &infos, int intg);
vector<int> find_entries_i_i (const multimap<int, int> &infos, int intg);
vector<string> find_entries_i_s (multimap<int, string>& infos, int intg);
void set_entries_d (multimap<int, double>& infos, int itr, vector<double> newval);

template <typename T, size_t N> T* end(T (&pArray)[N]);

vector<double> scale_a_vector_to_1(vector<double> a_vector);

void fill_map_from_specifications_i_i (istream& in, map<int, int>& infos, string namefolderinput);
void fill_map_from_specifications_i_s (istream& in, map<int, string>& infos, string namefolderinput);
void fill_map_from_specifications_i_d (istream& in, map<int, double>& infos, string namefolderinput);

// generator
//int my_rand(int a, int b)
//{
//    return a+ rand() % (b-a+1);
//}

double decode_the_tree(string& tree, vector<string>& direction,
map<string, int>& external_states, map<string, int>& internal_states);
#endif
