
// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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


#ifndef FISHERIESMANAGMENT_H
#define FISHERIESMANAGMENT_H

#include <Node.h>
#include <Population.h>
#include <Vessel.h>
#include <options.h>


using namespace std;

#include <errno.h>

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

bool computeTariffMapUpdate(
        const DynAllocOptions& dyn_alloc_sce,
        int isFirstDayOfYear,
        int isFirstDayOfMonth,
        int tstep,
        int nbpops,
        int nbmets,
        vector<Node* >& nodes,
        vector<Population* >& populations,
        vector<Vessel* >& vessels,
        vector<double> arbitary_breaks_for_tariff,
        vector<int> tariff_pop,
        double tariff_annual_hcr_percent_change,
        int update_tariffs_based_on_lpue_or_dpue_code,
        int freq_update_tariff_code,
        int total_amount_credited,
        multimap<string, double> reloaded_fcredits);



bool computeEffortMultiplier(vector<Population* >& populations,
                          vector<Vessel* >& vessels,
                          int nb_y_left_to_tgrt_year,
                          int HCR);

bool computeTAC(vector<Population* >& populations,
                int sp,
                int tstep,
                double multiOnTACconstraint,
                int HCR);


#endif // FISHERIESMANAGMENT_H
