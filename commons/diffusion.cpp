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

#include <stdio.h>
#include <stdlib.h>
#include <helpers.h>

#include <string>
#include <vector>

#include <Node.h>

// NAIVE DIFFUSION FOR NOW i.e. DIFFUSION IN EVERY DIRECTION....
// TODO: import each Env Forcing info as
// a tuple for 1-value on node 2-gradient on node param norm 3-gradient param alpha
// then diffuse from dep node toward the dest node according to the gradient info,
// caution: the dest node not expected to be always part of the neighbour nodes because norm can be > dist in-between node...
// (so the pseudo code would be: compute first the dest in continuous space, then assign back to the closest node.)


void diffuse_Nitrogen_in_every_directions(vector<Node*>&list_of_nodes, adjacency_map_t& adjacency_map)
{

   cout << "start diffusion for Nitrogen...." << endl;

    vector<types::NodeId> list_of_nodes_idx;
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
       list_of_nodes_idx.push_back(list_of_nodes.at(n)->get_idx_node());
       }
    random_shuffle (list_of_nodes.begin(), list_of_nodes.end() );
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
        auto idx_node=list_of_nodes.at(n)->get_idx_node();


        // get the N for this pop on this node
        double departure_Nitrogen = list_of_nodes.at(n)->get_Nitrogen();


        // get the list of neighbouring nodes
        vector<types::NodeId> neighbour_nodes;
        vertex_t u = idx_node.toIndex();
        // Visit each edge exiting u
        for (std::list<edge>::iterator edge_iter = adjacency_map[u].begin();
             edge_iter != adjacency_map[u].end();
             edge_iter++)
        {
            neighbour_nodes.push_back(types::NodeId(edge_iter->target));
        }

        std::unique(neighbour_nodes.begin(), neighbour_nodes.end());

        double coeff= 0.4; // HARDCODED

        // displace a proportion of N from departure node to neighbours nodes
        int count = neighbour_nodes.size();
               double depN=departure_Nitrogen;
               for (int nei=0; nei<count; ++nei)
                  {
                      double arrival_Nitrogen = list_of_nodes.at(nei)->get_Nitrogen();
                      double exchanged       = ((coeff*depN)/count);
                      arrival_Nitrogen       = arrival_Nitrogen + exchanged;
                      departure_Nitrogen     = departure_Nitrogen - exchanged;
                      list_of_nodes.at(nei)->setNitrogen(arrival_Nitrogen);//update arrival
                   }
               list_of_nodes.at(n)->setNitrogen(departure_Nitrogen ); //update departure


      } // node by node

  cout << "stop diffusion Nitrogen for this node...." << endl;
}






