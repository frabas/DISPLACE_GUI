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

#ifndef COMSTRUCTS_H
#define COMSTRUCTS_H

#include <options.h>

namespace displace {
namespace commons {

struct Scenario {
    DynAllocOptions dyn_alloc_sce;
    PopSceOptions dyn_pop_sce;
    std::string biolsce;
    int a_graph;
    int nrow_coord;
    int nrow_graph;
    int a_port;
    double graph_res;
    bool is_individual_vessel_quotas;
    bool use_dtrees;

    std::vector <int> tariff_pop;
    int freq_update_tariff_code;
    std::vector <double> arbitary_breaks_for_tariff;
    int total_amount_credited;

    std::string dt_go_fishing;
    std::string dt_choose_ground;
    std::string dt_start_fishing;
    std::string dt_change_ground;
    std::string dt_stop_fishing;
    std::string dt_change_port;



};

}
}

#endif // COMSTRUCTS_H

