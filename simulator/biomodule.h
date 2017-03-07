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


#ifndef BIOMODULE_H
#define BIOMODULE_H


#include <Population.h>
#include <Node.h>
#include <Vessel.h>
#include <options.h>

using namespace std;

#include <errno.h>

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif



int applyBiologicalModule(int tstep, const string &namesimu,
                          const string & namefolderinput, const string & namefolderoutput,	const string & pathoutput,
                          ofstream &popdyn_N,
                          ofstream &popdyn_F,
                          ofstream &popdyn_SSB,
                          ofstream &popdyn_annual_indic,
                          ofstream &popdyn_test2,
                          ofstream &popnodes_inc,
                          ofstream &popnodes_impact,
                          ofstream &popnodes_cumulcatches_per_pop,
                          ofstream &popnodes_cumftime,
                          ofstream &popnodes_cumsweptarea,
                          ofstream &popnodes_cumcatches,
                          ofstream &popnodes_tariffs,
                          ofstream &export_individual_tacs,
                          ofstream &popnodes_end,
                          ofstream &benthosbiomassnodes,
                          ofstream &benthosnumbernodes,
                          int nbbenthospops,
                          const char *path,
                          bool use_gnuplot,
                          bool use_gui,
                          const string & popdyn_N_filename,
                          const string & popdyn_F_filename,
                          const string & popdyn_SSB_filename,
                          const string & popnodes_inc_filename,
                          const string & popnodes_end_filename,
                          const string & popnodes_impact_filename,
                          const string & popnodes_cumulcatches_per_pop_filename,
                          const string & popnodes_cumftime_filename,
                          const string & popnodes_cumsweptarea_filename,
                          const string & popnodes_cumcatches_filename,
                          const string & popnodes_tariffs_filename,
                          const string & popnodes_benthos_biomass_filename,
                          const string & popnodes_benthos_number_filename,
                          const vector<int> & tsteps_quarters, const vector<int> & tsteps_semesters,
                          const vector<int> & tsteps_years, const vector<int> & tsteps_months,
                          const vector<int> & implicit_pops,
                          const vector<double> & calib_oth_landings,
                          multimap<int, int> &nodes_in_polygons,
                          bool is_tacs,
                          int export_vmslike,
                          int freq_do_growth,
                          const multimap<int,double> &init_weight_per_szgroup,
                          const vector<vector <double> > &species_interactions_mortality_proportion_matrix,
                          vector<Population* >& populations,
                          vector<Node* >&nodes,
                          vector<Vessel* >& vessels,
                          vector<Benthos* >& benthos,
                          const PopSceOptions &dyn_pop_sce,
                          const DynAllocOptions &dyn_alloc_sce);

#endif

