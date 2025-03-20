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

#ifndef BENTHOS_H
#define BENTHOS_H

#include <commons_global.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class  Benthos
{
	public:
        Benthos(int id,
                int marine_landscape,
            const int nbfuncgr,
            const vector<Node* > &nodes,
            const vector<double> &prop_funcgr_biomass_per_node,
            const vector<double> &prop_funcgr_number_per_node,
            const vector<double> &meanw_funcgr_number_per_node,
            const vector<double> &recovery_rates_per_funcgr,
            const vector<double> &benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,
            const vector<double> &benthos_number_carrying_capacity_K_per_landscape_per_funcgr,
            bool is_benthos_in_numbers, bool is_benthos_in_longevity_classes,
            const vector<double> &h_betas_per_pop,
            multimap<int, double> &_longevity_classes_condition_per_node);
        virtual ~Benthos();
        int get_id() const;
        int get_marine_landscape() const;
        const vector<Node* > &get_list_nodes() const;
        const vector<double> &get_prop_funcgr_biomass_per_node() const;
        const vector<double> &get_prop_funcgr_number_per_node() const;
        const vector<double> &get_meanw_funcgr_per_node() const;
        const vector<double> &get_recovery_rates_per_funcgr() const;
        const vector<double> &get_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr() const;
        const vector<double> &get_benthos_number_carrying_capacity_K_per_landscape_per_funcgr() const;
        void recover_benthos_tot_biomass_per_funcgroup(int is_longevity);
        void recover_benthos_tot_number_per_funcgroup();

        const vector<double> &get_h_betas_per_pop() const; // link to Vessel::do_catch() harvest function

    protected:
	private:
        int id;
        int marine_landscape;
								 // area distribution
		vector<Node* > list_nodes;
								 // total biomass per functional group
        vector<double> prop_funcgr_biomass_per_node;
        vector<double> prop_funcgr_number_per_node;
        vector<double> meanw_funcgr_per_node;
        vector<double> recovery_rates_per_funcgr;
        vector<double> benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
        vector<double> benthos_number_carrying_capacity_K_per_landscape_per_funcgr;

        vector<double> h_betas_per_pop;
};
#endif							 // BENTHOS_H
