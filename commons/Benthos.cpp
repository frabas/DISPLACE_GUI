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

//this is how we can able/disable cout in C++:
#ifdef VERBOSE
#define dout cout
#else
#define dout 0 && cout
#endif

#include "Benthos.h"

Benthos::Benthos(int _marine_landscape,
const vector<Node *> &_nodes,
const vector<double> &_tot_biomass)
{
	marine_landscape =_marine_landscape;
	tot_biomass    =_tot_biomass;

	cout << "for this landscape "<< marine_landscape <<", assigned nodes are: ";
	vector<Node* > p_spe_nodes;
	for(int n=0; n<_nodes.size(); n++)
	{
		if(_nodes[n]->get_marine_landscape()== marine_landscape)
		{
			p_spe_nodes.push_back (_nodes[  n  ]);
			cout << _nodes[  n  ]->get_idx_node() << " ";
		}
	}
	cout << endl;
	for(int i=0; i<p_spe_nodes.size(); i++)
	{
		list_nodes.push_back(p_spe_nodes[i]);
		for(int funcgr=0; funcgr<tot_biomass.size();funcgr++)
		{
								 // put an estimate of biomass per cell
			p_spe_nodes[i]->add_benthos_tot_biomass_on_node(tot_biomass.at(funcgr) );
		}
		dout << "nb func. grp. on this node " << p_spe_nodes[i]->get_idx_node() <<
			"this marine landscape " << marine_landscape << " is " << tot_biomass.size() << endl;

	}

}


Benthos::~Benthos()
{
	//dtor
}


int Benthos::get_marine_landscape() const
{
	return(marine_landscape);
}


const vector<double> &Benthos::get_tot_biomass() const
{
	return(tot_biomass);
}


const vector<Node *> &Benthos::get_list_nodes() const
{
	return(list_nodes);
}
