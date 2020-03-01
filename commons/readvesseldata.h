//
// Created by happycactus on 3/1/20.
//

#ifndef DISPLACE_READVESSELDATA_H
#define DISPLACE_READVESSELDATA_H

#include "idtypes.h"
#include "vesselcalendar.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

// vessel specific
bool read_vessels_features(string a_quarter,
                           vector<string> &vesselids,
                           vector<int> &vid_is_actives,
                           vector<int> &vid_is_part_of_ref_fleets,
                           vector<double> &speeds,
                           vector<double> &fuelcons,
                           vector<double> &lengths,
                           vector<double> &vKWs,
                           vector<double> &carrycapacities,
                           vector<double> &tankcapacities,
                           vector<double> &nbfpingspertrips,
                           vector<double> &resttime_par1s,
                           vector<double> &resttime_par2s,
                           vector<double> &av_trip_duration,
                           vector<double> &mult_fuelcons_when_steaming,
                           vector<double> &mult_fuelcons_when_fishing,
                           vector<double> &mult_fuelcons_when_returning,
                           vector<double> &mult_fuelcons_when_inactive,
                           vector<int> &firm_ids,
                           string folder_name_parameterization,
                           string inputfolder,
                           int selected_vessels_only, vector<VesselCalendar> &calendars);

bool read_vessels_economics_features(vector<string> &vesselids,
                                     vector<double> &this_vessel_nb_crews,
                                     vector<double> &annual_other_incomes,
                                     vector<double> &landing_costs_percents,
                                     vector<double> &crewshare_and_unpaid_labour_costs_percents,
                                     vector<double> &other_variable_costs_per_unit_efforts,
                                     vector<double> &annual_insurance_costs_per_crews,
                                     vector<double> &standard_labour_hour_opportunity_costss,
                                     vector<double> &standard_annual_full_time_employement_hourss,
                                     vector<double> &other_annual_fixed_costss,
                                     vector<double> &vessel_values,
                                     vector<double> &annual_depreciation_rates,
                                     vector<double> &opportunity_interest_rates,
                                     vector<double> &annual_discount_rates,
                                     string folder_name_parameterization,
                                     string inputfolder
);


multimap<string, types::NodeId>
read_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, types::NodeId>
read_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, types::NodeId>
read_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, double> read_freq_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, double>
read_freq_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, double> read_freq_harbours(string a_quarter, string folder_name_parameterization, string inputfolder);

multimap<string, double> read_vessels_betas(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<string, double> read_vessels_tacs(string a_semester, string folder_name_parameterization, string inputfolder);

multimap<string, double> read_initial_fishing_credits(string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, int>
read_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, double>
read_freq_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, double>
read_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder);

multimap<types::NodeId, double>
read_gshape_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization,
                                  string inputfolder);

multimap<types::NodeId, double>
read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization,
                                  string inputfolder);


#endif //DISPLACE_READVESSELDATA_H
