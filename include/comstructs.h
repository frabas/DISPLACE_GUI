// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <idtypes.h>
#include <commons_global.h>
#include <options.h>
#include <cmath>

namespace displace {
namespace commons {

struct  Scenario {
    DynAllocOptions dyn_alloc_sce;
    PopSceOptions dyn_pop_sce;
    ClosureOptions closure_opts;
    std::string biolsce;
    std::string fleetsce;
    int freq_do_growth;
    int freq_redispatch_the_pop;
    int a_graph;
    int nrow_coord;
    int nrow_graph;
    types::NodeId a_port;
    std::vector <double>  graph_res;
    bool is_individual_vessel_quotas;
    bool check_all_stocks_before_going_fishing;
    bool use_dtrees;

    std::vector <int> tariff_pop;
    int freq_update_tariff_code;
    std::vector <double> arbitary_breaks_for_tariff;
    int total_amount_credited;
    double tariff_annual_hcr_percent_change;
    int update_tariffs_based_on_lpue_or_dpue_code;

    std::string dt_go_fishing;
    std::string dt_choose_ground;
    std::string dt_start_fishing;
    std::string dt_change_ground;
    std::string dt_stop_fishing;
    std::string dt_change_port;


    friend bool operator == (const Scenario &s1, const Scenario &s2) {
        return
                s1.dyn_alloc_sce == s2.dyn_alloc_sce &&
                s1.dyn_pop_sce == s2.dyn_pop_sce &&
                s1.closure_opts == s2.closure_opts &&
                s1.biolsce == s2.biolsce &&
                s1.fleetsce == s2.fleetsce &&
                s1.freq_do_growth == s2.freq_do_growth &&
                s1.freq_redispatch_the_pop == s2.freq_redispatch_the_pop &&
                s1.a_graph == s2.a_graph &&
                s1.nrow_coord == s2.nrow_coord &&
                s1.nrow_graph == s2.nrow_graph &&
                s1.a_port == s2.a_port &&
                check(s1.graph_res, s2.graph_res) &&
                s1.is_individual_vessel_quotas == s2.is_individual_vessel_quotas &&
                s1.check_all_stocks_before_going_fishing == s2.check_all_stocks_before_going_fishing &&
                s1.use_dtrees == s2.use_dtrees &&

                s1.tariff_pop == s2.tariff_pop &&
                s1.freq_update_tariff_code == s2.freq_update_tariff_code &&
                check(s1.arbitary_breaks_for_tariff, s2.arbitary_breaks_for_tariff) &&
                s1.total_amount_credited == s2.total_amount_credited &&
                std::abs((double)(s1.tariff_annual_hcr_percent_change - s2.tariff_annual_hcr_percent_change)) < 1e-3 &&
                s1.update_tariffs_based_on_lpue_or_dpue_code == s2.update_tariffs_based_on_lpue_or_dpue_code &&

                s1.dt_go_fishing == s2.dt_go_fishing &&
                s1.dt_choose_ground == s2.dt_choose_ground &&
                s1.dt_start_fishing == s2.dt_start_fishing &&
                s1.dt_change_ground == s2.dt_change_ground &&
                s1.dt_stop_fishing == s2.dt_stop_fishing &&
                s1.dt_change_port == s2.dt_change_port;
    }

    friend bool operator != (const Scenario &s1, const Scenario &s2) {
        return !(s1 == s2);
    }

private:
    static bool check(const std::vector<double> &c1 , const std::vector<double> &c2) {
        if (c1.size() != c2.size()) return false;

        for (size_t i = 0; i < c1.size(); ++i) {
            if (std::abs((double)(c1[i]-c2[i])) > 1e-3)
                return false;
        }
        return true;
    }
};

}
}

#endif // COMSTRUCTS_H

