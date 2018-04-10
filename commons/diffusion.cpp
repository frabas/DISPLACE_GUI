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


#include <diffusion.h>


namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, unsigned> value;


void createRTreeFromNodes(vector<Node*>& nodes, bgi::rtree< value, bgi::quadratic<16> >& rtree){

    // create some values
    for ( unsigned i = 0 ; i < 10 ; ++i )
    {
        // create a box
        box b(point(i + 0.0f, i + 0.0f), point(i + 0.5f, i + 0.5f));
        // insert new value
        rtree.insert(std::make_pair(b, i));
    }

}

bool diffuse_Nitrogen_with_gradients(vector<Node*>&list_of_nodes, adjacency_map_t& adjacency_map, bgi::rtree< value, bgi::quadratic<16> >& rtree)
{
    cout << "start diffusion for Nitrogen along gradients...." << endl;


     vector<types::NodeId> list_of_nodes_idx;
     for (int n=0; n<list_of_nodes.size(); ++n)
        {
        list_of_nodes_idx.push_back(list_of_nodes.at(n)->get_idx_node());
        }
     random_shuffle (list_of_nodes_idx.begin(), list_of_nodes_idx.end() );
     for (int n=0; n<list_of_nodes_idx.size(); ++n)
        {
         auto idx_node=list_of_nodes_idx.at(n);


         // get the value on this node
         double departure_Nitrogen = list_of_nodes.at(n)->get_Nitrogen();

         // get the gradient value on this node
         double departure_Nitrogen_norm = list_of_nodes.at(n)->get_Nitrogen_norm();
         double departure_Nitrogen_alpha = list_of_nodes.at(n)->get_Nitrogen_alpha();



         // compute pos in polar space
         double lon = list_of_nodes.at(n)->get_x() + departure_Nitrogen_norm*cos((departure_Nitrogen_alpha*M_PI/180));
         double lat = list_of_nodes.at(n)->get_y() + departure_Nitrogen_norm*sin((departure_Nitrogen_alpha*M_PI/180));



         // find 5 nearest values to a point
         std::vector<value> result_n;
         rtree.query(bgi::nearest(point(0, 0), 5), std::back_inserter(result_n));

         std::cout << "knn query result:" << std::endl;
         BOOST_FOREACH(value const& v, result_n)
             std::cout << bg::wkt<box>(v.first) << " - " << v.second << std::endl;

/*

        // exchange the amount between dep and arr given a fixed coeff.
        double coeff =0.4; // HARDCODED
        double arrival_Nitrogen = list_of_nodes.at(nearest_node_idx)->get_Nitrogen();
        double exchanged       = coeff*departure_Nitrogen;
        arrival_Nitrogen       = arrival_Nitrogen + exchanged;
        departure_Nitrogen     = departure_Nitrogen - exchanged;
        list_of_nodes.at(nearest_node_idx)->setNitrogen(arrival_Nitrogen);//update arrival
        list_of_nodes.at(idx_node.toIndex())->setNitrogen(departure_Nitrogen ); //update departure


*/

         }



    return 0;
}


bool diffuse_Nitrogen_in_every_directions(vector<Node*>&list_of_nodes, adjacency_map_t& adjacency_map, double coeff)
{

   cout << "start diffusion for Nitrogen...." << endl;

    vector<types::NodeId> list_of_nodes_idx;
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
       list_of_nodes_idx.push_back(list_of_nodes.at(n)->get_idx_node());
       }
    random_shuffle (list_of_nodes_idx.begin(), list_of_nodes_idx.end() );
    for (int n=0; n<list_of_nodes_idx.size(); ++n)
       {
        auto idx_node=list_of_nodes_idx.at(n);


        // get value on this node
        double departure_Nitrogen = list_of_nodes.at(n)->get_Nitrogen();


        // get the list of neighbouring nodes
        //vector<types::NodeId> neighbour_nodes;
        vector<int> neighbour_nodes;
                vertex_t u = idx_node.toIndex();
        // Visit each edge exiting u
        for (std::list<edge>::iterator edge_iter = adjacency_map[u].begin();
             edge_iter != adjacency_map[u].end();
             edge_iter++)
        {
            //neighbour_nodes.push_back(types::NodeId(edge_iter->target));
            neighbour_nodes.push_back(edge_iter->target);
        }


        // apply unique
        std::sort(neighbour_nodes.begin(), neighbour_nodes.end());
        std::vector<int>::iterator last=std::unique(neighbour_nodes.begin(), neighbour_nodes.end());
        neighbour_nodes.erase(last, neighbour_nodes.end());


        // displace a proportion of N from departure node to neighbours nodes
        int count = neighbour_nodes.size();


         double depN=departure_Nitrogen;

         //if(idx_node.toIndex()==9006){
         //    cout << "on this node " <<  idx_node.toIndex() << " BEFORE departure_Nitrogen is "<< departure_Nitrogen << endl;
         //    for(int i=0; i< neighbour_nodes.size();i++) cout <<  neighbour_nodes.at(i) << " ";
         //    cout << endl;
         //}
               for (int nei=0; nei<count; ++nei)
                  {
                      double arrival_Nitrogen = list_of_nodes.at(neighbour_nodes.at(nei))->get_Nitrogen();
                      double exchanged       = ((coeff*depN)/count);
                      //if(idx_node.toIndex()==9006) cout << "on this node " <<  neighbour_nodes.at(nei) << " BEFORE arrival_Nitrogen is "<< arrival_Nitrogen << endl;
                      arrival_Nitrogen       = arrival_Nitrogen + exchanged;
                      //if(idx_node.toIndex()==9006) cout << "on this node " <<  neighbour_nodes.at(nei) << " AFTER arrival_Nitrogen is "<< arrival_Nitrogen << endl;
                      departure_Nitrogen     = departure_Nitrogen - exchanged;
                      list_of_nodes.at(neighbour_nodes.at(nei))->setNitrogen(arrival_Nitrogen);//update arrival
                   }
               list_of_nodes.at(idx_node.toIndex())->setNitrogen(departure_Nitrogen ); //update departure


          //if(idx_node.toIndex()==9006){
          //     cout << "on this node " <<  idx_node.toIndex() << " AFTER departure_Nitrogen is "<< departure_Nitrogen << endl;
          //     cout << endl;
          //}

      } // node by node

  cout << "stop diffusion Nitrogen for this node...." << endl;

  return 0;
}






