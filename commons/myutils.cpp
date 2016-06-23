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

#include"myutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <helpers.h>

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

double myintegrand(double x, double S1, double S2){
return 1/(1+exp((S1-S2*x)));
}


double trapezoidal(double a, double b, vector <double> sel){
double sum=sel[a]*0.5;
for (int i=a+1; i<b; i++) sum +=sel[i];
sum+=sel[b]*0.5;
return sum;
}



template <typename T, size_t N>
T* end(T (&pArray)[N])
{
	return &pArray[0] + N;
}


void remove_dups(vector<int>& seq)
{
	sort( seq.begin(), seq.end() ) ;
	seq.erase( unique( seq.begin(), seq.end() ), seq.end() ) ;
}


// to keep the first element of all the keys only:
multimap<int,int>  remove_dups(multimap<int,int>& original_map)
{
   multimap<int,int> new_map;

   while (original_map.size() > 0)
      {
       pair<int,int> element = *(original_map.begin());
       new_map.insert(make_pair(element.first,element.second));
       original_map.erase(element.first);
      }
return(new_map);
}


// to remove key-value duplicates:
multimap<int,int>::const_iterator find_pair(const multimap<int,int>& map, const pair<int, int>& pair)
{
    std::pair<multimap<int, int>::const_iterator ,multimap<int, int>::const_iterator > range = map.equal_range(pair.first);
    for (multimap<int, int>::const_iterator p = range.first; p != range.second; ++p)
        if (p->second == pair.second)
            return p;
    return map.end();
}

bool insert_if_not_present(multimap<int,int>& map, const pair<int, int>& pair)
{
    if (find_pair(map, pair) == map.end()) {
        map.insert(pair);
        return true;
    }
    return false;
}



/*
void print( vector <string> & v )
{
	for (size_t n = 0; n < v.size(); n++)
		cout << "\"" << v[ n ] << "\"\n";
	cout << endl;
}


void print_d( vector <double> & v )
{
	for (size_t n = 0; n < v.size(); n++)
		cout << "\"" << v[ n ] << "\"\n";
	cout << endl;
}
*/

//pair first less comparator>>=
template <typename T1, typename T2>
struct pair_first_less
{
	bool operator()(std::pair<T1,T2> p1, std::pair<T1,T2> p2) const
	{
		if(p1.first == p2.first)
		{
			//Otherwise the initial vertex_queue will have the size 2 { 0,source ; inf;n }
			return p1.second < p2.second;
		}
		return p1.first < p2.first;
	}
};

// TO DO: modify the Dijkstra algo to incorporate the potential
// effect of wind and current on the routing
// by eg adding penalty on edges proportinoal to their proximity with
// nodes of a map of winds and currents.....
// could be a column added to the graph object
// node 1 /node 2/ dist.km / wind force / current speed
// where the weight of the edge would then be a linear combination
//  of dist.km, wind force and current speed....

//simple compute paths function>>=
void DijkstraComputePaths(vertex_t source,
adjacency_map_t& adjacency_map,
std::map<vertex_t, weight_t>& min_distance,
std::map<vertex_t, vertex_t>& previous,
std::vector<int> relevant_nodes)
{
	//initialize output parameters>>
	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
		vertex_iter != adjacency_map.end();
		vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		//min_distance[v] = std::numeric_limits< double >::infinity();
		min_distance[v] = 10000;
	}

	min_distance[source] = 0;
	//visit each vertex u, always visiting vertex with smallest min_distance first>>
	std::set< std::pair<weight_t, vertex_t>,
		pair_first_less<weight_t, vertex_t> > vertex_queue;
	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
		vertex_iter != adjacency_map.end();
		vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));
	}
	while (!relevant_nodes.empty() && !vertex_queue.empty())
	{
		vertex_t u = vertex_queue.begin()->second;
		vertex_queue.erase(vertex_queue.begin());
		// Visit each edge exiting u
		for (std::list<edge>::iterator edge_iter = adjacency_map[u].begin();
			edge_iter != adjacency_map[u].end();
			edge_iter++)
		{
			vertex_t v = edge_iter->target;
								 // GET HERE THE DIST IN km (to the nearest km because an integer here)...
			weight_t weight = edge_iter->weight;
			//relax the edge (u,v)>>=
			weight_t distance_through_u = min_distance[u] + weight;
			//if (distance_through_u<700 && distance_through_u < min_distance[v])
			//if ( distance_through_u < min_distance[v])
			std::vector<int>::iterator invalid;
			if (distance_through_u < min_distance[v])
			{
				//remove v from queue>>
				vertex_queue.erase(std::pair<weight_t, vertex_t>(min_distance[v], v));
				min_distance[v] = distance_through_u;
				previous[v] = u;

				//re-add v to queue>>=
				vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));

			}

			// for speeding up the simu by reducing the size of the object "previous":
			// remove v from list of dest nodes if it is a dest node
			// then stop the search of paths if all dest nodes are reached...
			invalid =remove(relevant_nodes.begin(), relevant_nodes.end(), v);
			relevant_nodes.erase(invalid, relevant_nodes.end());

		}

		//if(relevant_nodes.size()<7){
		//    for (int i=0; i<relevant_nodes.size(); i++){
		//    std::cout << relevant_nodes.at(i) <<" ";
		//    }
		//}

	}

}


//get shortest path function>>=
std::list<vertex_t> DijkstraGetShortestPathTo(
vertex_t target, std::map<vertex_t, vertex_t>& previous)
{
	std::list<vertex_t> path;
	std::map<vertex_t, vertex_t>::iterator prev;
	vertex_t vertex = target;
	path.push_front(vertex);
	while((prev = previous.find(vertex)) != previous.end())
	{
		vertex = prev->second;
		path.push_front(vertex);
	}
	return path;
}


// remove the keys that are not used in the map "previous"
// according to all the potential destination nodes for a given source node
void SimplifyThePreviousMap(
int source,
std::map<vertex_t, vertex_t>& previous,
std::vector<int>& relevant_nodes,
std::map<vertex_t, weight_t>& min_distance,
string namesimu,
string a_graph_name,
string inputfolder)
{
	std::cout << "size before: " << previous.size()  << std::endl;

	std::list<vertex_t> paths;

	// 1. compute all paths from the source to the set of destinations
    for (unsigned int i=0; i<relevant_nodes.size(); i++)
	{
		std::list<vertex_t> path = DijkstraGetShortestPathTo(relevant_nodes[i], previous);

		//paths.merge(path); // concatenate
		// we dont care about the order so replaced by:
								 // concatenate
		paths.insert (paths.end(),path.begin(),path.end());

	}

	// 2. sort to speed up binary_search
	paths.sort();

	// 3. remove potential replicates (caution: the container need to be orderered first)
	paths.unique();

	//std::cout << "all nodes used: " << std::endl;
	//std::list<vertex_t>::iterator i;
	//for(i=paths.begin(); i != paths.end(); ++i) std::cout << *i << " ";
	//std::cout << std::endl;

	// 4. export but without the not used nodes (because they do not belong to at least one path)
	// export into a file
	std::ofstream a_previous_map;
	stringstream out;
	out << source;
	string source_s = out.str();
    std::string filename= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/previous_"+source_s+".dat";
	a_previous_map.open(filename.c_str());
	a_previous_map << " key " << " value " << std::endl;

	std::map<vertex_t, vertex_t>::iterator prev;
	for ( prev=previous.begin() ; prev != previous.end(); prev++ )
	{
		vertex_t vertex = prev->first;
		vertex_t vertex2 = prev->second;

		// check if the nodes of a line in 'previous' are in use (caution: on both columns)....
		if(binary_search(paths.begin(), paths.end(), vertex) &&
			binary_search(paths.begin(), paths.end(), vertex2))
		{
			a_previous_map << vertex  <<" " << vertex2 << std::endl;
            dout(cout  << "line " << vertex << " " << vertex2 << " is kept!" << endl);
		}

	}
	a_previous_map.close();

	std::ofstream a_min_distance;
    std::string filename2= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/min_distance_"+source_s+".dat";
	a_min_distance.open(filename2.c_str());
	a_min_distance << " key " << " value " << std::endl;

	std::map<vertex_t, weight_t>::iterator dis;
	for ( dis=min_distance.begin() ; dis != min_distance.end(); dis++ )
	{
		vertex_t vertex = dis->first;
		vertex_t distance = dis->second;
		if(binary_search(paths.begin(), paths.end(), vertex) )
		{
			a_min_distance << vertex  <<" " << distance << std::endl;
		}
		//std::cout << vertex << ": removed..." << std::endl;

	}
	a_min_distance.close();

}


// CURRENTLY NOT IN USE AND SHOULD BE CHECKED.....
// a function which alter the node connections by changing dist to 10000km!
// this is like closing the nodes for steaming (i.e. no shortest path will use them anymore...)
// CAUTION THIS ALTER THE ADJACENCY MATRIX FOR THE REST OF THE TIME!
// note that they will be kept if they are destination node. so this is not closing fishing.
void closeSomeNodes(std::vector<int>& nodes_to_be_closed, adjacency_map_t& adjacency_map)
{

	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
		vertex_iter != adjacency_map.end();
		vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		if(binary_search(nodes_to_be_closed.begin(), nodes_to_be_closed.end(), v))
		{

			for (std::list<edge>::iterator edge_iter = adjacency_map[v].begin();
				edge_iter != adjacency_map[v].end();
				edge_iter++)
			{
				edge_iter->weight = 100000;

			}
		}

	}
}


/**
fill in the separated vectors related to the coord object
@param the coord file, ...
*/
void fill_from_coord (istream& in, vector<double>& graph_coord_x,
vector<double> & graph_coord_y,
vector<int>& graph_coord_harbour, int nrow)
{
	double val;
	int val2;

	for (int i = 0; i < nrow; i++)
	{
		in >> val;
		graph_coord_x.push_back(val);
	}
	for (int i2=nrow; i2 < (nrow+nrow); i2++)
	{
		in >> val;
		graph_coord_y.push_back(val);
	}
	for (int i3=(nrow+nrow); i3 < (nrow+nrow+nrow); i3++)
	{
		in >> val2;
		graph_coord_harbour.push_back(val2);
	}

	cout << "read coord with "
		<< graph_coord_x.size() << " nodes" << endl << flush;
}


/**
fill in the separated vectors related to the coord object
@param the coord file, ...
*/
void fill_from_graph (istream& in, vector<int>& graph_idx_dep,
vector<int> & graph_idx_arr,
vector<int>& graph_dist_km, int nrow)
{
	double val;

	for (int i = 0; i < nrow; i++)
	{
		in >> val;
		graph_idx_dep.push_back(val);
        dout(cout  << "val dep: " << val << endl);
	}
	for (int i2=nrow; i2 < (nrow+nrow); i2++)
	{
		in >> val;
		graph_idx_arr.push_back(val);
        dout(cout  << "val arr: " << val << endl);
	}
	for (int i3=(nrow+nrow); i3 < (nrow+nrow+nrow); i3++)
	{
		in >> val;
								 // caution: convert into imt to speedup c++
		unsigned int val_int = static_cast<unsigned int>(val + 0.5);
		graph_dist_km.push_back(val_int);
	}

	cout << "read graph with "
		<< graph_idx_dep.size() << " connections " << endl << flush;
}


void fill_from_code_area (istream& in, vector<int>& graph_point_code_area, int nrow)
{
	double val;
	int val2;

	for (int i = 0; i < nrow; i++)
	{
		in >> val;				 // first column: do nothing...
	}
	for (int i2=nrow; i2 < (nrow+nrow); i2++)
	{
		in >> val;				 // second column: do nothing...
	}
	for (int i3=(nrow+nrow); i3 < (nrow+nrow+nrow); i3++)
	{
		in >> val2;
		graph_point_code_area.push_back(val2);
	}

    dout (cout << "read code area with "
        << graph_point_code_area.size() << " nodes" << endl << flush);
}


void fill_from_code_marine_landscape (istream& in, vector<int>& graph_point_code_landscape, int nrow)
{
	int val;
	cout << "landscape codes are: ";
	for (int i = 0; i < nrow; i++)
	{
		in >> val;
		graph_point_code_landscape.push_back(val);
		cout << val << " ";
	}

	cout << "read code marine landscape with "
		<< graph_point_code_landscape.size() << " nodes" << endl << flush;
}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
void fill_in_percent_szgroup_per_age_matrix (istream& in, vector< vector<double> >& percent_szgroup_per_age_matrix)
{
	double val;
	for(unsigned int i = 0; i < percent_szgroup_per_age_matrix.size(); i++)
	{
		for(unsigned int j = 0; j < percent_szgroup_per_age_matrix[i].size(); j++)
		{
			in>> val;
			percent_szgroup_per_age_matrix[i][j] = val;
		}
	}
    dout(cout  << "read size matrix percent_age_per_szgroup"  << endl << flush);
	for(unsigned int i = 0; i < percent_szgroup_per_age_matrix.size(); i++)
	{
		for(unsigned int j = 0; j < percent_szgroup_per_age_matrix[i].size(); j++)
		{
            dout(cout  << percent_szgroup_per_age_matrix[i][j] << " ");
		}
        dout(cout  << endl);
	}
    dout(cout  << endl);

}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
void fill_in_percent_age_per_szgroup_matrix (istream& in, vector< vector<double> >& percent_age_per_szgroup_matrix)
{
	double val;
	for(unsigned int i = 0; i < percent_age_per_szgroup_matrix.size(); i++)
	{
		for(unsigned int j = 0; j < percent_age_per_szgroup_matrix[i].size(); j++)
		{
			in>> val;
			percent_age_per_szgroup_matrix[i][j] = val;
		}
	}
    dout(cout  << "read size matrix percent_age_per_szgroup"  << endl << flush);
	for(unsigned int i = 0; i < percent_age_per_szgroup_matrix.size(); i++)
	{
		for(unsigned int j = 0; j < percent_age_per_szgroup_matrix[i].size(); j++)
		{
            dout(cout  << percent_age_per_szgroup_matrix[i][j] << " ");
		}
        dout(cout  << endl);
	}
    dout(cout  << endl);

}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
void fill_in_growth_transition (istream& in, vector< vector<double> >& growth_transition)
{
	double val;
	for(unsigned int i = 0; i < growth_transition.size(); i++)
	{
		for(unsigned int j = 0; j < growth_transition[i].size(); j++)
		{
			in>> val;
			growth_transition[i][j] = val;
		}
	}
    dout(cout  << "read size matrix transition"  << endl << flush);
	for(unsigned int i = 0; i < growth_transition.size(); i++)
	{
		for(unsigned int j = 0; j < growth_transition[i].size(); j++)
		{
            dout(cout  << growth_transition[i][j] << " ");
		}
        dout(cout  << endl);
	}
    dout(cout  << endl);

}


/**
fill in the species_interactions_mortality_proportion_matrix
@param the vector file, ...
*/
void fill_in_species_interactions_mortality_proportion_matrix (istream& in, vector< vector<double> >& species_interactions_mortality_proportion_matrix)
{
    double val;
    for(unsigned int i = 0; i < species_interactions_mortality_proportion_matrix.size(); i++)
    {
        for(unsigned int j = 0; j < species_interactions_mortality_proportion_matrix[i].size(); j++)
        {
            in>> val;
            species_interactions_mortality_proportion_matrix[i][j] = val;
        }
    }
    dout(cout  << "read species_interactions_mortality_proportion_matrix"  << endl << flush);
    for(unsigned int i = 0; i < species_interactions_mortality_proportion_matrix.size(); i++)
    {
        for(unsigned int j = 0; j < species_interactions_mortality_proportion_matrix[i].size(); j++)
        {
            dout(cout  << species_interactions_mortality_proportion_matrix[i][j] << " ");
        }
        dout(cout  << endl);
    }
    dout(cout  << endl);

}





/**
fill in the growth transition matrix
@param the vector file, ...
*/
void fill_in_selectivity_per_stock (istream& in, vector< vector<double> >& selectivity_per_stock)
{
    double val;
    for(unsigned int i = 0; i < selectivity_per_stock.size(); i++)
    {
        for(unsigned int j = 0; j < selectivity_per_stock[i].size(); j++)
        {
            in>> val;
            selectivity_per_stock[i][j] = val;
        }
    }
    dout(cout  << "read size selectivity_per_stock matrix"  << endl << flush);
    for(unsigned int i = 0; i < selectivity_per_stock.size(); i++)
    {
        for(unsigned int j = 0; j < selectivity_per_stock[i].size(); j++)
        {
            dout(cout  << selectivity_per_stock[i][j] << " ");
        }
        dout(cout  << endl);
    }
    dout(cout  << endl);

}



/**
fill in
@param the vector file, ...
*/
void fill_in_param_sr (istream& in, vector<double>& param_sr)
{
	double val;
	for(unsigned int i = 0; i < param_sr.size(); i++)
	{
		in>> val;
		param_sr[i] = val;
	}
    dout(cout  << "read param_sr"  << endl << flush);

	for(unsigned int i = 0; i < param_sr.size(); i++)
	{
        dout(cout  << param_sr[i] << " ");
	}
    dout(cout  << endl);

}


/**
fill in
@param the vector file, ...
*/
void fill_in_initial_tac (istream& in, vector<double>& initial_tac)
{
	double val;
	for(unsigned int i = 0; i < initial_tac.size(); i++)
	{
		in>> val;
		initial_tac[i] = val;
	}
    dout(cout  << "read initial_tac"  << endl << flush);

	for(unsigned int i = 0; i < initial_tac.size(); i++)
	{
        dout(cout  << initial_tac[i] << " ");
	}
    dout(cout  << endl);

}


/**
fill in
@param the vector file, ...
*/
void fill_in_fbar_ages_min_max (istream& in, vector<double>& fbar_ages_min_max)
{
	double val;
	for(unsigned int i = 0; i < fbar_ages_min_max.size(); i++)
	{
		in>> val;
		fbar_ages_min_max[i] = val;
	}
    dout(cout  << "read fbar_ages_min_max"  << endl << flush);

	for(unsigned int i = 0; i < fbar_ages_min_max.size(); i++)
	{
        dout(cout  << fbar_ages_min_max[i] << " ");
	}
    dout(cout  << endl);

}


/**
fill in the vessel attributes into a multimap DOUBLE
@param the vessel specification file, ...
*/
void fill_from_metier_specifications (istream& in, multimap<string, double>& infos)
{

	string line;
	while(!getline(in, line).eof())
	{
		string key;
		in >> key;
		double val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the metier ogive specification  " << endl << flush);
}


/**
fill in the vessel attributes
@param the vessel specification file, ...
*/
void fill_from_vessels_specifications (istream& in, vector<string>& names,
vector<double>& speeds, vector<double>& fuelcons, vector<double>& lengths, vector<double>& vKWs,
vector<double>& carrycapacities, vector<double>& tankcapacities,
vector<double>& nbfpingspertrips,
vector<double>& resttime_par1s, vector<double>& resttime_par2s,
                                       vector<double>& av_trip_duration,
                                       vector<double>& mult_fuelcons_when_steaming,
                                       vector<double>& mult_fuelcons_when_fishing,
                                       vector<double>& mult_fuelcons_when_returning,
                                       vector<double>& mult_fuelcons_when_inactive, vector<VesselCalendar> &calendars)
{

    std::string line;
    while(!getline(in, line).eof())
	{
        VesselCalendar calendar;

        vector<string> fields;

        boost::split(fields, line, boost::is_any_of("|"));

        if (fields.size() > 15) {
            try {
                calendar.weekEndStartDay = boost::lexical_cast<int>(fields[15]);
            } catch (boost::bad_lexical_cast &x) {}
        }
        if (fields.size() > 16) {
            try {
                calendar.weekEndEndDay = boost::lexical_cast<int>(fields[16]);
            } catch (boost::bad_lexical_cast &x) {}
        }
        if (fields.size() > 17) {
            try {
                calendar.workStartHour = boost::lexical_cast<int>(fields[17]);
            } catch (boost::bad_lexical_cast &x) {}
        }
        if (fields.size() > 18) {
            try {
                calendar.workEndHour = boost::lexical_cast<int>(fields[18]);
            } catch (boost::bad_lexical_cast &x) {}
        }

        names.push_back(fields[0]);
        speeds.push_back(strtod(fields[1].c_str(),0));
        fuelcons.push_back(strtod(fields[2].c_str(),0));
        lengths.push_back(strtod(fields[3].c_str(),0));
        vKWs.push_back(strtod(fields[4].c_str(),0));
        carrycapacities.push_back(strtod(fields[5].c_str(),0));
        tankcapacities.push_back(strtod(fields[6].c_str(),0));
        nbfpingspertrips.push_back(strtod(fields[7].c_str(),0));
        resttime_par1s.push_back(strtod(fields[8].c_str(),0));
        resttime_par2s.push_back(strtod(fields[9].c_str(),0));
        av_trip_duration.push_back(strtod(fields[10].c_str(),0));
        mult_fuelcons_when_steaming.push_back(strtod(fields[11].c_str(),0));
        mult_fuelcons_when_fishing.push_back(strtod(fields[12].c_str(),0));
        mult_fuelcons_when_returning.push_back(strtod(fields[13].c_str(),0));
        mult_fuelcons_when_inactive.push_back(strtod(fields[14].c_str(),0));

        calendars.push_back(calendar);
    }
    cout  << "read and set up the general features of each vessel...OK" << endl;

}


/**
fill in the vessel attributes
@param the vessel specification file, ...
*/
void fill_from_ships_specifications (istream& in,
                                     vector<string>& names,
                                     vector<double>& imos,
                                     vector<double>& yearbuilds,
                                     vector<string>& flags,
                                     vector<string>& types,
                                     vector<double>& typecodes,
                                     vector<double>& loas,
                                     vector<double>& KWs,
                                     vector<double>& breadths,
                                     vector<double>& grosstonnages,
                                     vector<double>& nbunits,
                                     vector<double>& fueluses,
                                     vector<double>& NOxEmission_gperKWhs,
                                     vector<double>& SOxEmission_percentpertotalfuelmasss,
                                     vector<double>& GHGEmissions,
                                     vector<double>& PMEmissions,
                                     vector<double>& vmaxs,
                                     vector<double>& vcruises,
                                     vector<double>& lane_ids)
{
	string name;
	while(!std::getline(in, name, '|').eof())
	{
        string imo;
        string yearbuild;
        string flag;
        string type;
        string typecode;
        string loa;
        string KW;
        string breadth;
        string grosstonnage;
        string nbunit;
        string fueluse;
        string NOxEmission_gperKWh;
        string SOxEmission_percentpertotalfuelmass;
        string GHGEmission;
        string PMEmission;
        string vmax;
		string vcruise;
		string lane_id;
        std::getline(in, imo, '|');
        std::getline(in, yearbuild, '|');
        std::getline(in, flag, '|');
        std::getline(in, type, '|');
        std::getline(in, typecode, '|');
        std::getline(in, loa, '|');
        std::getline(in, KW, '|');
        std::getline(in, breadth, '|');
        std::getline(in, grosstonnage, '|');
        std::getline(in, nbunit, '|');
        std::getline(in, fueluse, '|');
        std::getline(in, NOxEmission_gperKWh, '|');
        std::getline(in, SOxEmission_percentpertotalfuelmass, '|');
        std::getline(in, GHGEmission, '|');
        std::getline(in, PMEmission, '|');
        std::getline(in, vmax, '|');
        std::getline(in, vcruise, '|');
		std::getline(in, lane_id);
		names.push_back(name);
        imos.push_back(strtod(imo.c_str(),0));
        yearbuilds.push_back(strtod(yearbuild.c_str(),0));
        flags.push_back(flag);
        types.push_back(type);
        typecodes.push_back(strtod(typecode.c_str(),0));
        loas.push_back(strtod(loa.c_str(),0));
        KWs.push_back(strtod(KW.c_str(),0));
        breadths.push_back(strtod(breadth.c_str(),0));
        grosstonnages.push_back(strtod(grosstonnage.c_str(),0));
        nbunits.push_back(strtod(nbunit.c_str(),0));
        fueluses.push_back(strtod(fueluse.c_str(),0));
        NOxEmission_gperKWhs.push_back(strtod(NOxEmission_gperKWh.c_str(),0));
        SOxEmission_percentpertotalfuelmasss.push_back(strtod(SOxEmission_percentpertotalfuelmass.c_str(),0));
        GHGEmissions.push_back(strtod(GHGEmission.c_str(),0));
        PMEmissions.push_back(strtod(PMEmission.c_str(),0));
        vmaxs.push_back(strtod(vmax.c_str(),0));
		vcruises.push_back(strtod(vcruise.c_str(),0));
		lane_ids.push_back(strtod(lane_id.c_str(),0));
	}
    dout(cout  << "read and set up the general features of each ship...OK" << endl << flush);

}


/**
fill in the avai attributes into a map
@param the vessel specification file, ...
*/
void fill_from_avai_nodes_with_pop (istream& in, map<int, double>& avai)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
		avai[key]=val;
	}
    dout(cout  << "read pop avaibility per node " << endl << flush);
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
void fill_from_avai_szgroup_nodes_with_pop (istream& in, multimap<int, double>& avai)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
		avai.insert(make_pair(key,val));
	}
    dout(cout  << "read the availability at szgroup " << endl << flush);
}


/**
fill in the oth_land attributes into a multimap
@param the oth_land specification file, ...
*/
void fill_from_oth_land (istream& in, map<int, double>& oth_land)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
		oth_land.insert(make_pair(key,val));
	}
    dout(cout  << "read oth land " << endl << flush);
}

/**
fill in the overall_migration_fluxes attributes into a multimap
@param the overall_migration_fluxes specification file, ...
*/
void fill_from_overall_migration_fluxes (istream& in, multimap<int, double>& overall_migration_fluxes)
{

    string line;
    while(!getline(in, line).eof())
    {
        int key;
        in >> key;
        double val;
        in >> val;
        overall_migration_fluxes.insert(make_pair(key,val));
    }
    dout(cout  << "read overall_migration_fluxes " << endl << flush);
}


/**
fill in the attributes into a multimap
@param the specification file, ...
*/
void fill_from_relative_stability (istream& in, map<string, double>& relative_stability)
{

	string line;
	while(!getline(in, line).eof())
	{
		string key;
		in >> key;
		double val;
		in >> val;
		relative_stability.insert(make_pair(key,val));
	}
    dout(cout  << "read relative stability " << endl << flush);
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
void fill_map_from_specifications_i_i (istream& in, map<int, int>& a_map)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		int val;
		in >> val;
		a_map.insert(make_pair(key,val));
	}
    dout(cout  << "read a map <int,int> " << endl << flush);
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
void fill_map_from_specifications_i_s (istream& in, map<int, string>& a_map)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		string val;
		in >> val;
		a_map.insert(make_pair(key,val));
	}
    dout(cout  << "read a map <int,string> " << endl << flush);
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
void fill_from_nodes_in_polygons (istream& in, multimap<int, int>& nodes_in_polygons)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
		nodes_in_polygons.insert(make_pair(key,val));
	}
    dout(cout  << "read nodes_in_polygons " << endl << flush);
}


/**
fill in the vessel attributes into a multimap <string, integer>
@param e.g. the vessel specification file, ...
*/
void fill_multimap_from_specifications_s_i (istream& in, multimap<string, int>& infos)
{

	string line;
	while(!getline(in, line).eof())
	{
		string key;
		in >> key;
		int val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification <string, integer> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a multimap <integer, double>
@param e.g. the vessel specification file, ...
*/
void fill_multimap_from_specifications_i_d (istream& in, multimap<int, double> &infos)
{

	string line;
    while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification <int, double> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a multimap <string, double>
@param e.g. the vessel specification file, ...
*/
void fill_multimap_from_specifications_s_d (istream& in, multimap<string, double>& infos)
{

	string line;
    while(!getline(in, line).eof())
    {
		string key;
		in >> key;
		double val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification <string, double> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a multimap <int, string>
@param e.g. the vessel specification file, ...
*/
void fill_multimap_from_specifications_i_s (istream& in, multimap<int, string>& infos)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		string val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification <int, string> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a multimap <int, int>
@param e.g. the vessel specification file, ...
*/
void fill_multimap_from_specifications_i_i (istream& in, multimap<int, int>& infos)
{

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		int val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification multimap<int, int> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
void fill_map_from_specifications_i_i (istream& in, map<int, int>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		int val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification map<int, int> " << endl << flush);

	// TO DO: test if infos.count(key) = NBSZGROUP
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
void fill_map_from_specifications_i_d (istream& in, map<int, double>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);

	string line;
	while(!getline(in, line).eof())
	{
		int key;
		in >> key;
		double val;
		in >> val;
        infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification map<int, double> " << endl << flush);

}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
void fill_map_from_specifications_i_s (istream& in, map<int, string>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);

	string line;
	while(!getline(in, line).eof())
	{
		int key;
        in >> key;
		string val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification map<int, string> " << endl << flush);

}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
void fill_map_from_specifications_s_d (istream& in, map<string, double>& infos)
{

	string line;
	while(!getline(in, line).eof())
	{
		string key;
		in >> key;
		double val;
		in >> val;
		infos.insert(make_pair(key,val));
	}
    dout(cout  << "read and set up the specification map<int, double> " << endl << flush);

}


vector<double> find_entries_s_d (multimap<string, double>& infos, string str)
{
	multimap<string, double>::iterator lower
		= infos.lower_bound(str);
	multimap<string, double>::iterator upper
		= infos.upper_bound(str);
	vector<double> result;

	for(multimap<string, double>::iterator pos = lower;
		pos!=upper; pos++)
	{
		result.push_back(pos->second);
	}
	return(result);
}


vector<int> find_entries_s_i (multimap<string, int>& infos, string str)
{
	multimap<string, int>::iterator lower
		= infos.lower_bound(str);
	multimap<string, int>::iterator upper
		= infos.upper_bound(str);
	vector<int> result;

	for(multimap<string, int>::iterator pos = lower;
		pos!=upper; pos++)
	{
		result.push_back(pos->second);
	}
	return(result);
}


vector<double> find_entries_i_d (const multimap<int, double>& infos, int intg)
{

    multimap<int, double>::const_iterator upper
		= infos.upper_bound(intg);
	vector<double> result;

    for(multimap<int, double>::const_iterator pos= infos.lower_bound(intg);
		pos!=upper; pos++)
	{
		result.push_back(pos->second);
	}

	return(result);
}


vector<int> find_entries_i_i (const multimap<int, int>& infos, int intg)
{
    multimap<int, int>::const_iterator lower
		= infos.lower_bound(intg);
    multimap<int, int>::const_iterator upper
		= infos.upper_bound(intg);
	vector<int> result;

    for(multimap<int, int>::const_iterator pos = lower;
		pos!=upper; pos++)
	{
		result.push_back(pos->second);
	}
	return(result);
}


vector<string> find_entries_i_s (multimap<int, string>& infos, int intg)
{
	multimap<int, string>::iterator lower
		= infos.lower_bound(intg);
	multimap<int, string>::iterator upper
		= infos.upper_bound(intg);
	vector<string> result;

	for(multimap<int, string>::iterator pos = lower;
		pos!=upper; pos++)
	{
		result.push_back(pos->second);
	}
	return(result);
}


void set_entries_d (multimap<int, double>& infos, int itr, vector<double> newval)
{
    dout(cout  << "set_entries_d...BEGIN" << endl);

	multimap<int, double>::iterator lower
		= infos.lower_bound(itr);
	multimap<int, double>::iterator upper
		= infos.upper_bound(itr);

	int i=0;
	for(multimap<int, double>::iterator pos = lower;
		pos!=upper; pos++)
	{
		pos->second = newval[i];
		i++;
	}
    dout(cout  << "set_entries_d...END" << endl);

}


vector<double> compute_distance_fgrounds(const vector <int>& idx_path_shop,
const deque<map<vertex_t, vertex_t> >& path_shop,
const deque<map<vertex_t, weight_t> >& min_distance_shop,
int from,
vector<int> grounds)
{
    vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), from);
								 // tricky!
	int idx = it - idx_path_shop.begin();

	map<vertex_t, vertex_t> previous = path_shop.at(idx);
	//std::list<map<int,int> >::iterator it_p = path_shop.begin();
	// advance(it_p, idx-1);
	//map<vertex_t, vertex_t> previous= *it_p;

	map<vertex_t, weight_t> min_distance = min_distance_shop.at(idx);
	// std::list<map<int,int> >::iterator it_d = min_distance_shop.begin();
	// advance(it_d, idx-1);
	// map<vertex_t, weight_t> min_distance= *it_d;

	vector <double> distance_fgrounds;
	for (unsigned int i=0; i<grounds.size(); i++)
	{
		vertex_t vx = grounds.at(i);
		distance_fgrounds.push_back(min_distance[vx]);
        dout(cout  << "distance to fishing ground " << min_distance[vx] << endl);
	}
	return(distance_fgrounds);
}


vector<double> scale_a_vector_to_1(vector<double> a_vector)
{
	// do the cumul....
	double cum_a_vector=0;
    for(unsigned int a_element = 0; a_element < a_vector.size(); a_element++)
	{
								 //
		cum_a_vector +=a_vector.at(a_element);
	}

	// ...then, scale to 1
	vector <double> freq_a_vector (a_vector.size());
	if(cum_a_vector!=0)
	{
        for(unsigned int a_node = 0; a_node < a_vector.size(); a_node++)
		{
			if(a_vector.at(a_node)==0)  cout << "a_vector.at(a_node) at 0 !! " << endl;
			freq_a_vector.at(a_node)= a_vector.at(a_node) / cum_a_vector;
            dout(cout  << "scaled a_vector is then " << freq_a_vector.at(a_node) << endl);
		}
	}
	else
	{

		cout << "cum_a_vector = 0 !! " << endl;

	}
	return(freq_a_vector);
}


//--------------------------------------------------------------------------------------//
//-------------------------related to the decision trees--------------------------------//
//--------------------------------------------------------------------------------------//

double decode_the_tree(string& tree, vector<string>& direction,
map<string, int>& external_states, map<string, int>& internal_states)
{

	vector<string> a_split_string;

    //dout(cout  << tree << endl);
    //dout(cout  << "the first delimiter is " << direction[0] << endl);
	//if(direction.size() != (external_states.size()+internal_states.size()))
	//{
    //    dout(cout  << "pble: check the reading_direction object..." << endl);
	//}
    for (unsigned int i=0; i<direction.size(); i++)
	{
		string a_delimiter =direction[i];
		int a_state;
		if(external_states.count(a_delimiter)>0)
		{
								 // get the current status of the drivers....
			a_state = external_states.find(a_delimiter)->second;
		}
		else
		{
								 // external not found, search for internal states instead.
			a_state = internal_states.find(a_delimiter)->second;
		}
		//cout << "a tree " << tree << endl;
		//cout << "a delimiter " << a_delimiter << endl;
		//cout << "a state " << a_state << endl;
								 // split the tree in 2 according to the driver
		split( a_split_string, tree, a_delimiter, splitX::no_empties);
		//print(a_split_string);
		//   system("PAUSE");
								 // keep only one branch (0 left or 1 right) and loop...
		if(a_split_string.size()>1) tree = a_split_string[a_state];

		//cout << "the tree is now reduced to " << tree << endl;
	}
	//cout << "the found value is: " << tree << endl;

	return(atof(tree.c_str()));
}
