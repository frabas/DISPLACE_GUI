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
            const vector<Node* > &nodes,
            const vector<double> &prop_funcgr_per_node,
            const vector<double> &recovery_rates_per_funcgr,
            const vector<double> &benthos_carrying_capacity_K_per_landscape_per_funcgr);
		virtual ~Benthos();
		int get_marine_landscape() const;
        const vector<Node* > &get_list_nodes() const;
        const vector<double> &get_prop_funcgr_per_node() const;
        const vector<double> &get_recovery_rates_per_funcgr() const;
        const vector<double> &get_benthos_carrying_capacity_K_per_landscape_per_funcgr() const;
        void recover_benthos_tot_biomass_per_funcgroup();

    protected:
	private:
		int marine_landscape;
								 // area distribution
		vector<Node* > list_nodes;
								 // total biomass per functional group
        vector<double> prop_funcgr_per_node;
        vector<double> recovery_rates_per_funcgr;
        vector<double> benthos_carrying_capacity_K_per_landscape_per_funcgr;
};
#endif							 // BENTHOS_H
