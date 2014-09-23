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

#ifndef BENTHOS_H
#define BENTHOS_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class Benthos
{
	public:
		Benthos(int marine_landscape,
			vector<Node* > nodes,
			vector<double> tot_biomass);
		virtual ~Benthos();
		int get_marine_landscape() const;
		vector<Node* > get_list_nodes() const;
		vector<double> get_tot_biomass() const;
	protected:
	private:
		int marine_landscape;
								 // area distribution
		vector<Node* > list_nodes;
								 // total biomass per functional group
		vector<double> tot_biomass;
};
#endif							 // BENTHOS_H
