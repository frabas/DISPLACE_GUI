
// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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


#ifndef BIOMODULE2_H
#define BIOMODULE2_H

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



bool applyBiologicalModule2(int tstep, int a_month_i, int a_quarter_i, int a_year_i, const string &namesimu,
                          const string & namefolderinput, const string & namefolderoutput,	const string & pathoutput,
                          ofstream & popstats,
                          ofstream &popdyn_N,
                          ofstream &popdyn_F,
                          ofstream &popdyn_SSB,
                          ofstream &popdyn_annual_indic,
                          ofstream &popnodes_inc,
                          ofstream &popnodes_impact,
                          ofstream &popnodes_cumulcatches_per_pop,
                          ofstream &nodes_envt,
                          ofstream &popnodes_cumftime,
                          ofstream &popnodes_cumsweptarea,
                          ofstream &popnodes_cumcatches,
                          ofstream &popnodes_cumcatches_with_threshold,
                          ofstream &popnodes_cumdiscards,
                          ofstream &popnodes_cumdiscardsratio,
                          ofstream &popnodes_nbchoked,
                          ofstream &popnodes_tariffs,
                          ofstream &export_individual_tacs,
                          ofstream &popnodes_end,
                          ofstream &benthosbiomassnodes,
                          ofstream &benthosnumbernodes,
                          int nbbenthospops,
                          int nb_mets,
                          bool use_gui,
                          const string & popstats_filename,
                          const string & popdyn_N_filename,
                          const string & popdyn_F_filename,
                          const string & popdyn_SSB_filename,
                          const string & popnodes_inc_filename,
                          const string & popnodes_end_filename,
                          const string & popnodes_impact_filename,
                          const string & popnodes_cumulcatches_per_pop_filename,                         
                          const string & nodes_envt_filename,
                          const string & popnodes_cumftime_filename,
                          const string & popnodes_cumsweptarea_filename,
                          const string & popnodes_cumcatches_filename,
                          const string & popnodes_cumcatches_with_threshold_filename,
                          const string & popnodes_cumdiscards_filename,
                          const string & popnodes_cumdiscardsratio_filename,
                          const string & popnodes_nbchoked_filename,
                          const string & popnodes_tariffs_filename,
                          const string & popnodes_benthos_biomass_filename,
                          const string & popnodes_benthos_number_filename,
                          const vector<int> & tsteps_quarters, const vector<int> & tsteps_semesters,
                          const vector<int> & tsteps_years, const vector<int> & tsteps_months,
                          const vector<int> & implicit_pops,
                          const vector<double> & calib_oth_landings,
                          vector<vector <double> >& selectivity_per_stock_ogives_for_oth_land,
                          bool is_tacs,
                          bool is_other_land_as_multiplier_on_sp,
                          bool is_oth_land_per_metier,
                          int export_vmslike,
                          int freq_do_growth,
                          const multimap<int,double> &init_weight_per_szgroup,
                          const vector<vector <double> > &species_interactions_mortality_proportion_matrix,
                          vector<Population* >& populations,
                          vector<Node* >&nodes,
                          vector<Vessel* >& vessels,
                          vector<Benthos* >& benthos,
                          const PopSceOptions &dyn_pop_sce,
                          const DynAllocOptions &dyn_alloc_sce,
                          vector<vector<double> > &Ws_at_szgroup,
                          vector<vector<vector<vector<double> > > > &predKernel,
                          vector<vector<double> > &searchVolMat,
                          vector<vector<double> > &juveniles_diet_preference,
                          vector<vector<double> > &adults_diet_preference,
                          vector<int> &  mat_cats
                           );


#endif // BIOMODULE2_H
