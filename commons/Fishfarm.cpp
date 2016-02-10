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

#include<string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Fishfarm.h"
#include <helpers.h>

Fishfarm::Fishfarm(int a_name,
                       const vector<Node *> &nodes,
                       const multimap <int, double> _fishfarm_size)
{

    name=a_name;

    fishfarm_size= _fishfarm_size;

    //
    vector<Node* > p_spe_nodes;
    for(multimap<int, double>::iterator iter=fishfarm_size.begin(); iter != fishfarm_size.end();
        iter = fishfarm_size.upper_bound( iter->first ) )
    {
        p_spe_nodes.push_back (nodes[  iter->first  ]);
        // nodes[ iter->first ]->set_fishfarm_names_on_node(a_name);

    }
    for(unsigned int i=0; i<p_spe_nodes.size(); i++)
        list_nodes.push_back(p_spe_nodes[i]);



}




Fishfarm::~Fishfarm()
{
    //dtor
}

int Fishfarm::get_name() const
{
    return(name);
}


const vector<Node* > &Fishfarm::get_list_nodes() const
{
    return(list_nodes);
}


void Fishfarm::set_list_nodes(vector<Node* > _list_nodes)
{
    list_nodes= _list_nodes;
}

