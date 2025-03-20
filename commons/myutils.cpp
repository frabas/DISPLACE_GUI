// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <utils/MultifieldReader.h>
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
                          spp::sparse_hash_map<vertex_t, weight_t> &min_distance,
                          spp::sparse_hash_map<vertex_t, vertex_t> &previous,
                          std::vector<types::NodeId> relevant_nodes)
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
            auto invalid =remove(relevant_nodes.begin(), relevant_nodes.end(), types::NodeId(v));
            relevant_nodes.erase(invalid, relevant_nodes.end());

        }


    }

}


//get shortest path function>>=
std::list<vertex_t> DijkstraGetShortestPathTo(
        vertex_t target, spp::sparse_hash_map<vertex_t, vertex_t>& previous)
{
    std::list<vertex_t> path;
    spp::sparse_hash_map<vertex_t, vertex_t>::iterator prev;
    vertex_t vertex = target;
    path.push_front(vertex);
    while((prev = previous.find(vertex)) != previous.end())
    {
        vertex = prev->second;
        path.push_front(vertex);
    }
    return path;
}

std::list<types::NodeId> DijkstraGetShortestPathTo(types::NodeId target, const PathShop &pathshop)
{
    std::list<types::NodeId> path;
    auto vertex = target;
    path.push_front(vertex);
    while (true) {
        try {
            auto node = pathshop.getNode(vertex);
            vertex = node.getPreviousNode();
            if (!types::isIdInvalid(vertex))
                path.push_front(vertex);
            else
                break;
        } catch (PathShop::NodeNotFoundException &) {
            // end of path
            break;
        }
    }

    return path;
}


// remove the keys that are not used in the map "previous"
// according to all the potential destination nodes for a given source node
void SimplifyThePreviousMap(
        int source,
        spp::sparse_hash_map<vertex_t, vertex_t>& previous,
        std::vector<types::NodeId>& relevant_nodes,
        spp::sparse_hash_map<vertex_t, weight_t>& min_distance,
        string namesimu,
        string a_graph_name,
        string inputfolder)
{
    std::cout << "size before: " << previous.size()  << std::endl;

    std::list<vertex_t> paths;

    // 1. compute all paths from the source to the set of destinations
    for (unsigned int i=0; i<relevant_nodes.size(); i++)
    {
        std::list<vertex_t> path = DijkstraGetShortestPathTo(relevant_nodes[i].toIndex(), previous);

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

    spp::sparse_hash_map<vertex_t, vertex_t>::iterator prev;
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

    spp::sparse_hash_map<vertex_t, weight_t>::iterator dis;
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
bool fill_from_coord(istream& in, vector<double>& graph_coord_x,
                      vector<double> & graph_coord_y,
                      vector<int>& graph_coord_harbour, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_coord_x.push_back(val);
            } else if (linenum < 2*nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_coord_y.push_back(val);
            } else if (linenum < 3*nrow) {
                int val = boost::lexical_cast<int>(line);
                graph_coord_harbour.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_coord file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


/**
fill in the separated vectors related to the coord object
@param the coord file, ...
*/
bool fill_from_graph(istream& in, vector<int>& graph_idx_dep,
                      vector<int> & graph_idx_arr,
                      vector<int>& graph_dist_km, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_idx_dep.push_back(val);
            } else if (linenum < 2*nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_idx_arr.push_back(val);
            } else if (linenum < 3*nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_dist_km.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_graph file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_code_area(istream& in, vector<int>& graph_point_code_area, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < 2*nrow) {
                // ignore first two fields (groups really)
            } else if (linenum < 3*nrow) {
                int val = boost::lexical_cast<int>(line);
                graph_point_code_area.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_code_area file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_code_marine_landscape(istream& in, vector<int>& graph_point_code_landscape, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                int val = boost::lexical_cast<int>(line);
                graph_point_code_landscape.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on graph_point_code_landscape file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}

bool fill_from_wind(istream& in, vector<double>& graph_point_wind, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_wind.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on graph_point_wind file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}

bool fill_from_sst(istream& in, vector<double>& graph_point_sst, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_sst.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on graph_point_sst file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}

bool fill_from_salinity(istream& in, vector<double>& graph_point_salinity, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_salinity.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_salinity file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_Nitrogen(istream& in, vector<double>& graph_point_Nitrogen, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_Nitrogen.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_Nitrogen file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_Phosphorus(istream& in, vector<double>& graph_point_Phosphorus, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_Phosphorus.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_Phosphorus file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_Oxygen(istream& in, vector<double>& graph_point_Oxygen, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_Oxygen.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_Oxygen file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_DissolvedCarbon(istream& in, vector<double>& graph_point_DissolvedCarbon, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_DissolvedCarbon.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_DissolvedCarbon file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_bathymetry(istream& in, vector<double>& graph_point_bathymetry, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_bathymetry.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_bathymetry file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_shippingdensity(istream& in, vector<double>& graph_point_shippingdensity, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<int>(line);
                graph_point_shippingdensity.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_shippingdensity file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}

bool fill_from_siltfraction(istream& in, vector<double>& graph_point_siltfraction, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_point_siltfraction.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_siltfraction file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


bool fill_from_icesrectanglecode(istream& in, vector<double>& graph_point_icesrectanglecode, int nrow)
{
    int linenum = 0;
    graph_point_icesrectanglecode.clear();

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_point_icesrectanglecode.push_back(val);
            }
            else {
                break;  // finish.
            }
            ++linenum;
        }
    }
    catch (boost::bad_lexical_cast & ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_icesrectanglecode file line " << linenum <<
            " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}



bool fill_from_benthos_biomass(istream& in, vector<double>& graph_point_benthos_biomass, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_point_benthos_biomass.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on fill_from_benthos_biomass file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}

bool fill_from_benthos_number(istream& in, vector<double>& graph_point_benthos_number, int nrow)
{
    int linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            if (linenum < nrow) {
                double val = boost::lexical_cast<double>(line);
                graph_point_benthos_number.push_back(val);
            } else {
                break;  // finish.
            }
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on graph_point_benthos_number file line " << linenum <<
                " : " << ex.what() << "\n";
#endif
        return false;
    }

    return true;
}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
bool fill_in_percent_szgroup_per_age_matrix(istream& in, vector< vector<double> >& percent_szgroup_per_age_matrix)
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

    return true;
}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
bool fill_in_percent_age_per_szgroup_matrix(istream& in, vector< vector<double> >& percent_age_per_szgroup_matrix)
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

    return true;
}


/**
fill in the growth transition matrix
@param the vector file, ...
*/
bool fill_in_growth_transition(istream& in, vector< vector<double> >& growth_transition)
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

    return true;
}




/**
fill in the preferences_for_species_matrix
@param the vector file, ...
*/
bool fill_in_preferences_for_species_matrix (istream& in, vector< vector<double> >& preferences_for_species_matrix)
{
    double val;
    for(unsigned int i = 0; i < preferences_for_species_matrix.size(); i++)
    {
        for(unsigned int j = 0; j < preferences_for_species_matrix[i].size(); j++)
        {
            in>> val;
            preferences_for_species_matrix[i][j] = val;
        }
    }


    dout(cout  << "read preferences_for_species_matrix"  << endl << flush);
    for(unsigned int i = 0; i < preferences_for_species_matrix.size(); i++)
    {
        for(unsigned int j = 0; j < preferences_for_species_matrix[i].size(); j++)
        {
            dout(cout  << preferences_for_species_matrix[i][j] << " ");
        }
        dout(cout  << endl);
    }
    dout(cout  << endl);

    return true;
}



/**
fill in the species_interactions_mortality_proportion_matrix
@param the vector file, ...
*/
bool fill_in_species_interactions_mortality_proportion_matrix (istream& in, vector< vector<double> >& species_interactions_mortality_proportion_matrix)
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

    return true;
}





/**
fill in the growth transition matrix
@param the vector file, ...
*/
bool fill_in_selectivity_per_stock(istream& in, vector< vector<double> >& selectivity_per_stock)
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

    return true;
}



/**
fill in
@param the vector file, ...
*/
bool fill_in_param_sr(istream& in, vector<double>& param_sr)
{
    param_sr.clear();
    try {
        while (in) {

            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            double val = boost::lexical_cast<double>(line);
            param_sr.emplace_back (val);
        }
    } catch (boost::bad_lexical_cast &ex) {
        return false;
    }

    while(param_sr.size()<4) param_sr.emplace_back (0.0);

    cout  << "read param_sr"  << endl;

    for(unsigned int i = 0; i < param_sr.size(); i++)
    {
        cout  << param_sr[i] << " ";
    }
    cout  << endl;

    return true;
}


/**
fill in
@param the vector file, ...
*/
bool fill_in_initial_tac(istream& in, vector<double>& initial_tac)
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

    return true;
}


/**
fill in
@param the vector file, ...
*/
bool fill_in_fbar_ages_min_max(istream& in, vector<double>& fbar_ages_min_max)
{
    fbar_ages_min_max.clear();
    try {
        while (!in.eof())
        {
            std::string field;
            in >> field;
            boost::trim(field);
            if (!field.empty()) {
                fbar_ages_min_max.emplace_back (boost::lexical_cast<double>(field));
            }
        }

#if 0
        dout(cout  << "read fbar_ages_min_max"  << endl << flush);

        for(unsigned int i = 0; i < fbar_ages_min_max.size(); i++)
        {
            dout(cout  << fbar_ages_min_max[i] << " ");
        }
        dout(cout  << endl);
#endif

        return true;

    } catch (boost::bad_lexical_cast &) {
        return false;
    }
}


/**
fill in the vessel attributes into a multimap DOUBLE
@param the vessel specification file, ...
*/
bool fill_from_metier_specifications(istream& in, multimap<string, double>& infos)
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
    return true;
}


/**
fill in the vessel attributes
@param the vessel specification file, ...
*/
bool fill_from_vessels_specifications (istream& in,
                                       vector<string>& names,
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
                                       vector<VesselCalendar> &calendars)
{

    std::string line;
    try {
        while(!in.eof())
        {
            getline(in, line);
            boost::trim(line);
            if (line.empty())
                continue;

            VesselCalendar calendar;

            vector<string> fields;

            boost::split(fields, line, boost::is_any_of("|"));

            if (fields.size() < 22) {
              cout << "missing fields when loading vessel features!!" << endl;
                return false;   // all fields are MANDATORY.
            }

            calendar.weekEndStartDay = boost::lexical_cast<int>(fields[16]);
            calendar.weekEndEndDay = boost::lexical_cast<int>(fields[17]);
            calendar.workStartHour = boost::lexical_cast<int>(fields[18]);
            calendar.workEndHour = boost::lexical_cast<int>(fields[19]);

            tout(cout << "Reading vessel features for " << fields[0] << endl);
            names.push_back(fields[0]);
            vid_is_actives.push_back(boost::lexical_cast<int>(fields[1].c_str()));
            speeds.push_back(boost::lexical_cast<double>(fields[2].c_str()));
            fuelcons.push_back(boost::lexical_cast<double>(fields[3].c_str()));
            lengths.push_back(boost::lexical_cast<double>(fields[4].c_str()));
            vKWs.push_back(boost::lexical_cast<double>(fields[5].c_str()));
            carrycapacities.push_back(boost::lexical_cast<double>(fields[6].c_str()));
            tankcapacities.push_back(boost::lexical_cast<double>(fields[7].c_str()));
            nbfpingspertrips.push_back(boost::lexical_cast<double>(fields[8].c_str()));
            resttime_par1s.push_back(boost::lexical_cast<double>(fields[9].c_str()));
            resttime_par2s.push_back(boost::lexical_cast<double>(fields[10].c_str()));
            av_trip_duration.push_back(boost::lexical_cast<double>(fields[11].c_str()));
            mult_fuelcons_when_steaming.push_back(boost::lexical_cast<double>(fields[12].c_str()));
            mult_fuelcons_when_fishing.push_back(boost::lexical_cast<double>(fields[13].c_str()));
            mult_fuelcons_when_returning.push_back(boost::lexical_cast<double>(fields[14].c_str()));
            mult_fuelcons_when_inactive.push_back(boost::lexical_cast<double>(fields[15].c_str()));
            firm_ids.push_back(boost::lexical_cast<int>(fields[20].c_str()));
            vid_is_part_of_ref_fleets.push_back(boost::lexical_cast<int>(fields[21].c_str()));

            calendars.push_back(calendar);
        }
    } catch (boost::bad_lexical_cast &x) {
        return false;
    }

    tout(cout  << "read and set up the general features of each vessel...OK" << endl);

    return true;
}



/**
fill in the vessel attributes
@param the vessel specification file, ...
*/
bool fill_from_vessels_economic_specifications (istream& in,
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
                                                vector<double>& annual_discount_rates)
{

    std::string line;
    try {
        while(!in.eof())
        {
            getline(in, line);
            boost::trim(line);
            if (line.empty())
                continue;

            vector<string> fields;

            boost::split(fields, line, boost::is_any_of("|"));

            if (fields.size() < 14) {
                return false;   // all fields are MANDATORY.
            }

            this_vessel_nb_crews.push_back(boost::lexical_cast<double>(fields[1].c_str()));
            annual_other_incomes.push_back(boost::lexical_cast<double>(fields[2].c_str()));
            landing_costs_percents.push_back(boost::lexical_cast<double>(fields[3].c_str()));
            crewshare_and_unpaid_labour_costs_percents.push_back(boost::lexical_cast<double>(fields[4].c_str()));
            other_variable_costs_per_unit_efforts.push_back(boost::lexical_cast<double>(fields[5].c_str()));
            annual_insurance_costs_per_crews.push_back(boost::lexical_cast<double>(fields[6].c_str()));
            standard_labour_hour_opportunity_costss.push_back(boost::lexical_cast<double>(fields[7].c_str()));
            standard_annual_full_time_employement_hourss.push_back(boost::lexical_cast<double>(fields[8].c_str()));
            other_annual_fixed_costss.push_back(boost::lexical_cast<double>(fields[9].c_str()));
            vessel_values.push_back(boost::lexical_cast<double>(fields[10].c_str()));
            annual_depreciation_rates.push_back(boost::lexical_cast<double>(fields[11].c_str()));
            opportunity_interest_rates.push_back(boost::lexical_cast<double>(fields[12].c_str()));
            annual_discount_rates.push_back(boost::lexical_cast<double>(fields[13].c_str()));

        }
    } catch (boost::bad_lexical_cast &x) {
        return false;
    }

    dout(cout  << "read and set up the general features of each vessel...OK" << endl);

    return true;
}



/**
fill in the vessel attributes
@param the vessel specification file, ...
*/
bool fill_from_ships_specifications (istream& in,
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
    try {
        std::string line;
        while (!in.eof()) {
            getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            vector<string> fields;

            boost::split(fields, line, boost::is_any_of("|"));
            if (fields.size() < 19)
                return false;

            string name = fields[0];
            double imo = boost::lexical_cast<double>(fields[1]);
            double yearbuild = boost::lexical_cast<double>(fields[2]);
            string flag = fields[3];
            string type = fields[4];
            double typecode = boost::lexical_cast<double>(fields[5]);
            double loa = boost::lexical_cast<double>(fields[6]);
            double KW = boost::lexical_cast<double>(fields[7]);
            double breadth = boost::lexical_cast<double>(fields[8]);
            double grosstonnage = boost::lexical_cast<double>(fields[9]);
            double nbunit = boost::lexical_cast<double>(fields[10]);
            double fueluse = boost::lexical_cast<double>(fields[11]);
            double NOxEmission_gperKWh = boost::lexical_cast<double>(fields[12]);
            double SOxEmission_percentpertotalfuelmass = boost::lexical_cast<double>(fields[13]);
            double GHGEmission = boost::lexical_cast<double>(fields[14]);
            double PMEmission = boost::lexical_cast<double>(fields[15]);
            double vmax = boost::lexical_cast<double>(fields[16]);
            double vcruise = boost::lexical_cast<double>(fields[17]);
            double lane_id = boost::lexical_cast<double>(fields[18]);
            names.push_back(name);
            imos.push_back(imo);
            yearbuilds.push_back(yearbuild);
            flags.push_back(flag);
            types.push_back(type);
            typecodes.push_back(typecode);
            loas.push_back(loa);
            KWs.push_back(KW);
            breadths.push_back(breadth);
            grosstonnages.push_back(grosstonnage);
            nbunits.push_back(nbunit);
            fueluses.push_back(fueluse);
            NOxEmission_gperKWhs.push_back(NOxEmission_gperKWh);
            SOxEmission_percentpertotalfuelmasss.push_back(SOxEmission_percentpertotalfuelmass);
            GHGEmissions.push_back(GHGEmission);
            PMEmissions.push_back(PMEmission);
            vmaxs.push_back(vmax);
            vcruises.push_back(vcruise);
            lane_ids.push_back(lane_id);
        }
    } catch (boost::bad_lexical_cast &) {
        return false;
    }

    dout(cout  << "read and set up the general features of each ship...OK" << endl << flush);

    return true;
}



/**
fill in the firms attributes
@param the firm specification file, ...
*/
bool fill_from_firms_specifications (istream& in,
                                     vector<int> & firm_ids,
                                     vector<string> & firm_names,
                                     vector<int> & nb_vessels,
                                     vector<double> & longs,
                                     vector<double> & lats)
{
    try {
        std::string line;
        while (!in.eof()) {
            getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            vector<string> fields;

            boost::split(fields, line, boost::is_any_of("|"));
            if (fields.size() < 5)
                return false;

            int id      = boost::lexical_cast<int>(fields[0]);
            string name = fields[1];
            double nb   = boost::lexical_cast<int>(fields[2]);
            double lon  = boost::lexical_cast<double>(fields[3]);
            double lat  = boost::lexical_cast<double>(fields[4]);
            firm_ids.push_back(id);
            firm_names.push_back(name);
            nb_vessels.push_back(nb);
            longs.push_back(lon);
            lats.push_back(lat);
        }
    } catch (boost::bad_lexical_cast &) {
        return false;
    }

    dout(cout << "read and set up the firm features of each firm...OK" << endl << flush);

    return true;
}

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


/**
fill in the fishfarms attributes
@param the fishfarm specification file, ...
*/
bool fill_from_fishfarms_specifications(istream &in,
                                        vector<int> &fishfarms_ids,
                                        vector<string> &fishfarms_names,
                                        vector<int> &idx_nodes,
                                     vector<int> & is_actives,
                                     vector<double> & sizes,
                                     vector<double> & longs,
                                     vector<double> & lats,
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
                                     vector<double>& annual_profits
                                         )
{
    try {
        std::string line;
        while (!in.eof()) {
            getline(in, line);

            boost::trim(line);
            if (line.empty())
                continue;

            vector<string> fields;

            boost::split(fields, line, boost::is_any_of("|"));
            if (fields.size() < 5)
                return false;

            int id      = boost::lexical_cast<int>(fields[0]);
            string name = fields[1];
            double n    = boost::lexical_cast<int>(fields[54]);
            int is_active  = boost::lexical_cast<int>(fields[55]);
            double size = boost::lexical_cast<double>(fields[2]);
            double lon  = boost::lexical_cast<double>(fields[3]);
            double lat  = boost::lexical_cast<double>(fields[4]);
            double mean_SST= boost::lexical_cast<double>(fields[5]);
            double mean_salinity= boost::lexical_cast<double>(fields[6]);
            double mean_windspeed= boost::lexical_cast<double>(fields[7]);
            double mean_currentspeed= boost::lexical_cast<double>(fields[8]);
            double max_depth= boost::lexical_cast<double>(fields[9]);
            double diss_O2_mg_per_l= boost::lexical_cast<double>(fields[10]);
            double Linf_mm= boost::lexical_cast<double>(fields[11]);
            double K_y= boost::lexical_cast<double>(fields[12]);
            double t0_y= boost::lexical_cast<double>(fields[13]);
            double fulton_condition_factor= boost::lexical_cast<double>(fields[14]);
            string meanw_growth_model_type= boost::lexical_cast<string>(fields[15]);
            double start_day_growing= boost::lexical_cast<double>(fields[16]);
            int end_day_harvest= boost::lexical_cast<int>(fields[17]);
            int nbyears_for_growth= boost::lexical_cast<int>(fields[18]);
            int nb_days_fallowing_period= boost::lexical_cast<int>(fields[19]);
            int nb_fish_at_start= boost::lexical_cast<int>(fields[20]);
            double meanw_at_start= boost::lexical_cast<double>(fields[21]);
            double price_per_kg_at_start= boost::lexical_cast<double>(fields[22]);
            double target_meanw_at_harvest= boost::lexical_cast<double>(fields[23]);
            int nb_fish_at_harvest= boost::lexical_cast<int>(fields[24]);
            double meanw_at_harvest= boost::lexical_cast<double>(fields[25]);
            double prop_harvest_kg_sold= boost::lexical_cast<double>(fields[26]);
            double kg_eggs_per_kg= boost::lexical_cast<double>(fields[27]);
            double price_eggs_per_kg= boost::lexical_cast<double>(fields[28]);
            double N_in_fish_kg_3per= boost::lexical_cast<double>(fields[29]);
            double P_in_fish_kg_0_5per= boost::lexical_cast<double>(fields[30]);
            string feed_type= boost::lexical_cast<string>(fields[31]);
            double feed_price_per_kg= boost::lexical_cast<double>(fields[32]);
            double total_feed_kg= boost::lexical_cast<double>(fields[33]);
            double prop_N_in_feed= boost::lexical_cast<double>(fields[34]);
            double prop_P_in_feed= boost::lexical_cast<double>(fields[35]);
            double total_feed_N_kg= boost::lexical_cast<double>(fields[36]);
            double total_feed_P_kg= boost::lexical_cast<double>(fields[37]);
            string feed_type_vet= boost::lexical_cast<string>(fields[38]);
            double feed_vet_price_per_kg= boost::lexical_cast<double>(fields[39]);
            double total_feed_vet_kg= boost::lexical_cast<double>(fields[40]);
            double prop_N_in_feed_vet= boost::lexical_cast<double>(fields[41]);
            double prop_P_in_feed_vet= boost::lexical_cast<double>(fields[42]);
            double total_feed_vet_N_kg= boost::lexical_cast<double>(fields[43]);
            double total_feed_vet_P_kg= boost::lexical_cast<double>(fields[44]);
            double annual_discharge_N_kg= boost::lexical_cast<double>(fields[45]);
            double annual_discharge_P_kg= boost::lexical_cast<double>(fields[46]);
            double annual_discharge_C_kg= boost::lexical_cast<double>(fields[47]);
            double annual_discharge_heavymetals_kg= boost::lexical_cast<double>(fields[48]);
            double annual_discharge_medecine_kg= boost::lexical_cast<double>(fields[49]);
            double net_harvest_kg_per_sqkm_y= boost::lexical_cast<double>(fields[50]);
            double market_price_sold_fish= boost::lexical_cast<double>(fields[51]);
            double operating_cost_per_day= boost::lexical_cast<double>(fields[52]);
            double annual_profit= boost::lexical_cast<double>(fields[53]);

            fishfarms_ids.push_back(id);
            fishfarms_names.push_back(name);
            idx_nodes.push_back(n);
            is_actives.push_back(is_active);
            sizes.push_back(size);
            longs.push_back(lon);
            lats.push_back(lat);
            mean_SSTs.push_back(mean_SST);
            mean_salinities.push_back(mean_salinity);
            mean_windspeeds.push_back(mean_windspeed);
            mean_currentspeeds.push_back(mean_currentspeed);
            max_depths.push_back(max_depth);
            diss_O2_mg_per_ls.push_back(diss_O2_mg_per_l);
            Linf_mms.push_back(Linf_mm);
            K_ys.push_back(K_y);
            t0_ys.push_back(t0_y);
            fulton_condition_factors.push_back(fulton_condition_factor);
            meanw_growth_model_types.push_back(meanw_growth_model_type);
            start_day_growings.push_back(start_day_growing);
            end_day_harvests.push_back(end_day_harvest);
            nbyears_for_growths.push_back(nbyears_for_growth);
            nb_days_fallowing_periods.push_back(nb_days_fallowing_period);
            nb_fish_at_starts.push_back(nb_fish_at_start);
            meanw_at_starts.push_back(meanw_at_start);
            price_per_kg_at_starts.push_back(price_per_kg_at_start);
            target_meanw_at_harvests.push_back(target_meanw_at_harvest);
            nb_fish_at_harvests.push_back(nb_fish_at_harvest);
            meanw_at_harvests.push_back(meanw_at_harvest);
            prop_harvest_kg_solds.push_back(prop_harvest_kg_sold);
            kg_eggs_per_kgs.push_back(kg_eggs_per_kg);
            price_eggs_per_kgs.push_back(price_eggs_per_kg);
            N_in_fish_kg_3pers.push_back(N_in_fish_kg_3per);
            P_in_fish_kg_0_5pers.push_back(P_in_fish_kg_0_5per);
            feed_types.push_back(feed_type);
            feed_price_per_kgs.push_back(feed_price_per_kg);
            total_feed_kgs.push_back(total_feed_kg);
            prop_N_in_feeds.push_back(prop_N_in_feed);
            prop_P_in_feeds.push_back(prop_P_in_feed);
            total_feed_N_kgs.push_back(total_feed_N_kg);
            total_feed_P_kgs.push_back(total_feed_P_kg);
            feed_type_vets.push_back(feed_type_vet);
            feed_vet_price_per_kgs.push_back(feed_vet_price_per_kg);
            total_feed_vet_kgs.push_back(total_feed_vet_kg);
            prop_N_in_feed_vets.push_back(prop_N_in_feed_vet);
            prop_P_in_feed_vets.push_back(prop_P_in_feed_vet);
            total_feed_vet_N_kgs.push_back(total_feed_vet_N_kg);
            total_feed_vet_P_kgs.push_back(total_feed_vet_P_kg);
            annual_discharge_N_kgs.push_back(annual_discharge_N_kg);
            annual_discharge_P_kgs.push_back(annual_discharge_P_kg);
            annual_discharge_C_kgs.push_back(annual_discharge_C_kg);
            annual_discharge_heavymetals_kgs.push_back(annual_discharge_heavymetals_kg);
            annual_discharge_medecine_kgs.push_back(annual_discharge_medecine_kg);
            net_harvest_kg_per_sqkm_ys.push_back(net_harvest_kg_per_sqkm_y);
            market_price_sold_fishs.push_back(market_price_sold_fish);
            operating_cost_per_days.push_back(operating_cost_per_day);
            annual_profits.push_back(annual_profit);

            }
    } catch (boost::bad_lexical_cast &) {
        return false;
    }

    dout(cout  << "read and set up the fishfarms features of each farm...OK" << endl << flush);

    return true;
}



/**
fill in the avai attributes into a map
@param the vessel specification file, ...
*/
bool fill_from_avai_nodes_with_pop(istream& in, map<int, double>& avai)
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
    return true;
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
bool fill_from_avai_szgroup_nodes_with_pop(istream& in, multimap<types::NodeId, double>& avai)
{
    return fill_multimap_from_specifications<types::NodeId, double>(in, avai);
}

/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
bool fill_field_of_coeff_diffusion_this_pop(istream& in, multimap<types::NodeId, double>& coeffs)
{

    string line;
    while(!getline(in, line).eof())
    {
        int key;
        in >> key;
        double val;
        in >> val;
        coeffs.insert(make_pair(types::NodeId(key),val));
    }
    dout(cout  << "read the coeff of diffusion at szgroup " << endl << flush);
    return true;
}



/**
fill in the oth_land attributes into a multimap
@param the oth_land specification file, ...
*/
bool fill_from_oth_land(istream& in, map<types::NodeId, double>& oth_land)
{

    string line;
    while(!getline(in, line).eof())
    {
        int key;
        in >> key;
        double val;
        in >> val;
        oth_land.insert(make_pair(types::NodeId(key),val));
    }
    dout(cout  << "read oth land " << endl << flush);
    return true;
}

/**
fill in the overall_migration_fluxes attributes into a multimap
@param the overall_migration_fluxes specification file, ...
*/
bool fill_from_overall_migration_fluxes(istream& in, multimap<int, double>& overall_migration_fluxes)
{
    return fill_multimap_from_specifications_i_d(in, overall_migration_fluxes);
}


/**
fill in the attributes into a multimap
@param the specification file, ...
*/
bool fill_from_relative_stability(istream& in, map<string, double>& relative_stability)
{
    return fill_map_from_specifications_s_d(in, relative_stability);
}

/**
fill in the attributes into a multimap
@param the specification file, ...
*/
bool fill_from_percent_tac_per_vessel_length_class(istream& in, map<int, double>& percent_tac_per_vessel_length_class, string folder_name_parameterization)
{
    return fill_map_from_specifications_i_d(in, percent_tac_per_vessel_length_class, folder_name_parameterization);
}


/**
fill in the attributes into a multimap
@param the specification file, ...
*/
bool fill_from_percent_tac_cumul_over_months_key(istream& in, map<int, double>& percent_tac_cumul_over_months_key, string folder_name_parameterization)
{
    return fill_map_from_specifications_i_d(in, percent_tac_cumul_over_months_key, folder_name_parameterization);
}




/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
bool fill_map_from_specifications_i_i (istream& in, map<int, int>& a_map)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,int>>(in, " ", [&a_map](std::tuple<int, int> v) {
            a_map.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
bool fill_map_from_specifications_i_s (istream& in, map<int, string>& a_map)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,string>>(in, " ", [&a_map](std::tuple<int, string> v) {
            a_map.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the avai attributes into a multimap
@param the avai specification file, ...
*/
bool fill_from_nodes_in_polygons(istream& in, multimap<int, int>& nodes_in_polygons)
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

    return true;
}


/**
fill in the vessel attributes into a multimap <string, integer>
@param e.g. the vessel specification file, ...
*/
bool fill_multimap_from_specifications_s_i (istream& in, multimap<string, int>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<string,int>>(in, " ", [&infos](std::tuple<string, int> v) {
            infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/** @brief fill in the vessel attributes into a multimap <integer, double>. First line is an heading.
@param e.g. the vessel specification file, ...
*/
bool fill_multimap_from_specifications_i_d (istream& in, multimap<int, double> &infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,double>>(in, " ", [&infos](std::tuple<int, double> v) {
            infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}

/**
fill in the vessel attributes into a multimap <string, double>
@param e.g. the vessel specification file, ...
*/
bool fill_multimap_from_specifications_s_d (istream& in, multimap<string, double>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<string,double>>(in, " ", [&infos](std::tuple<string, double> v) {
            infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a multimap <int, string>
@param e.g. the vessel specification file, ...
*/
bool fill_multimap_from_specifications_i_s (istream& in, multimap<int, string>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,string>>(in, " ", [&infos](std::tuple<int, string> v) {
                                                              infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a multimap <int, int>
@param e.g. the vessel specification file, ...
*/
bool fill_multimap_from_specifications_i_i (istream& in, multimap<int, int>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,int>>(in, " ", [&infos](std::tuple<int, int> v) {
                                                           infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
bool fill_map_from_specifications_i_i (istream& in, map<int, int>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,int>>(in, " ", [&infos](std::tuple<int, int> v) {
                                                           infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
bool fill_map_from_specifications_i_d (istream& in, map<int, double>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,double>>(in, " ", [&infos](std::tuple<int, double> v) {
                                                              infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
bool fill_map_from_specifications_i_s (istream& in, map<int, string>& infos, string namefolderinput)
{
    UNUSED(namefolderinput);
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<int,string>>(in, " ", [&infos](std::tuple<int, string> v) {
                                                              infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }
}


/**
fill in the vessel attributes into a map <int, int>
@param e.g. the vessel specification file, ...
*/
bool fill_map_from_specifications_s_d (istream& in, map<string, double>& infos)
{
    std::string dummystring;
    getline (in, dummystring); // eat the heading

    displace::formats::helpers::MultifieldReader reader;
    try {
        return reader.importFromStream<std::tuple<std::string,double>>(in, " ", [&infos](std::tuple<string, double> v) {
                                                              infos.insert(std::make_pair(std::get<0>(v), std::get<1>(v)));
        });
    } catch (runtime_error &) {
        return false;
    }

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


vector<double> compute_distance_fgrounds(const vector<types::NodeId> &relevant_nodes,
                                         const std::vector<PathShop> &pathshops,
                                         types::NodeId from,
                                         vector<types::NodeId> grounds)
{
    outc (cout  << "look at the distances " << endl);

    vector<types::NodeId>::const_iterator it = find (relevant_nodes.begin(), relevant_nodes.end(), from);
    int idx = it - relevant_nodes.begin();

    outc (cout  << "look at the distances from node " <<   from.toIndex() << endl);

    vector <double> distance_fgrounds;
    for (unsigned int i=0; i<grounds.size(); i++)
      {
      double dist=1.0;
      auto vertex = grounds.at(i);
      distance_fgrounds.push_back(dist);

      while (true) {
        try {
            auto node = pathshops.at(idx).getNode(vertex);
            vertex = node.getPreviousNode();
            dist   = node.getWeight();
            if (!types::isIdInvalid(vertex))
                distance_fgrounds.at(i)+=dist;
            else
                break;
        } catch (PathShop::NodeNotFoundException &) {
            // end of path
            break;
        }
      }
    }

    outc(cout  << "look at the distances from node " << from.toIndex()  <<"  ...OK "<< endl);

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
    vector <double> freq_a_vector (a_vector.size(), 0.000001);
    if(cum_a_vector!=0)
    {
        for(unsigned int a_node = 0; a_node < a_vector.size(); a_node++)
        {
            //if(a_vector.at(a_node)==0)  cout << "a_vector.at(a_node) at 0 !! " << endl;
            freq_a_vector.at(a_node)= a_vector.at(a_node) / cum_a_vector;
            dout(cout  << "scaled a_vector is then " << freq_a_vector.at(a_node) << endl);
        }
    }
    else
    {
        //cout << "cum_a_vector = 0 !! " << endl;
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
        if (a_split_string.size() > 1) { tree = a_split_string[a_state]; }

        //cout << "the tree is now reduced to " << tree << endl;
    }
    //cout << "the found value is: " << tree << endl;

    return (atof(tree.c_str()));
}

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
)
{

    string filename = inputfolder + "/shipsspe_" + folder_name_parameterization + "/shipsspe_features.dat";

    ifstream ships_features;
    ships_features.open(filename.c_str());
    if (ships_features.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }

    bool r = fill_from_ships_specifications(ships_features, shipids, imos,
                                            yearbuilds, flags, types, typecodes,
                                            loas, KWs, breadths, grosstonnages, nbunits,
                                            fueluses, NOxEmission_gperKWhs,
                                            SOxEmission_percentpertotalfuelmasss,
                                            GHGEmissions, PMEmissions,
                                            vmaxs, vcruises, lane_ids);
    ships_features.close();

    return r;
}

multimap<int, double> read_shiplanes_lat(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/shipsspe_" + folder_name_parameterization + "/shipsspe_lanes_lat.dat";

    ifstream file_shipsspe_lanes_lat;
    file_shipsspe_lanes_lat.open(filename.c_str());
    if (file_shipsspe_lanes_lat.fail()) {
        cout << "Unfortunately the /shipsspe_lanes_lat.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> shipsspe_lanes_lat;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lat, shipsspe_lanes_lat);
    file_shipsspe_lanes_lat.close();

    return (shipsspe_lanes_lat);
}


multimap<int, double> read_shiplanes_lon(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/shipsspe_" + folder_name_parameterization + "/shipsspe_lanes_lon.dat";

    ifstream file_shipsspe_lanes_lon;
    file_shipsspe_lanes_lon.open(filename.c_str());
    if (file_shipsspe_lanes_lon.fail()) {
        cout << "Unfortunately the /shipsspe_lanes_lon.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> shipsspe_lanes_lon;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lon, shipsspe_lanes_lon);
    file_shipsspe_lanes_lon.close();

    return (shipsspe_lanes_lon);
}

map<int, double> read_size_per_windmill(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/windmillsspe_" + folder_name_parameterization + "/size_per_windmill.dat";

    ifstream file_size_per_windmill;
    file_size_per_windmill.open(filename.c_str());
    if (file_size_per_windmill.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> size_per_windmill;
    fill_map_from_specifications(file_size_per_windmill, size_per_windmill);
    file_size_per_windmill.close();

    return (size_per_windmill);
}

