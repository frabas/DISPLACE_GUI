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

#include "readdata.h"
#include <helpers.h>
#include <idtypes.h>
#include <idtypeshelpers.h>

#include <utils/LineNumberReader.h>
#include <boost/lexical_cast.hpp>
#include <utils/vectorsdata.h>
#include <boost/algorithm/string.hpp>

#include <numeric> // iota()

#include <legacy/binarygraphfilereader.h>

using namespace displace::formats;

#define NBSZGROUP 14
#define NBAGE 11				 // nb age classes max

/*
prints file opening error message.
@param filename, ...
*/

static std::string error_msg;

void open_file_error(string filename)
{
    error_msg = "error opening file " + filename;
    cout << error_msg << "\n";

    throw std::runtime_error(error_msg);
}

/**
read the settings for the siums given the case study
@param the vectors to be filled in, ...
*/
bool read_config_file(string folder_name_parameterization,
                      string inputfolder,
                      int& nbpops,
                      int& nbbenthospops,
                      vector<int>& implicit_pops,
                      vector<int>& implicit_pops_level2,
                      vector<int>& grouped_tacs,
                      vector<int>& nbcp_coupling_pops,
                      vector<double>& calib_oth_landings,
                      vector<double>& calib_w,
                      vector<double>& calib_cpue,
                      vector<types::NodeId> &interesting_harbours)
{

    string filename = inputfolder+"/simusspe_"+folder_name_parameterization+"/config.dat";
    std::cout << "Reading config file from " << filename << std::endl;

    std::ifstream fstream (filename.c_str(), std::ios_base::in);
    return read_config_file(fstream, nbpops, nbbenthospops, implicit_pops, implicit_pops_level2, grouped_tacs, nbcp_coupling_pops, calib_oth_landings,
                            calib_w, calib_cpue, interesting_harbours);
}

bool read_config_file(std::istream &stream,
                      int& nbpops,
                      int& nbbenthospops,
                      vector<int>& implicit_pops,
                      vector<int>& implicit_pops_level2,
                      vector<int>& grouped_tacs,
                      vector<int>& nbcp_coupling_pops,
                      vector<double>& calib_oth_landings,
                      vector<double>& calib_w,
                      vector<double>& calib_cpue,
                      vector<types::NodeId> &interesting_harbours)
{
    helpers::LineNumberReader reader;
    static const helpers::LineNumberReader::Specifications specs {
        {1,"nbpops"},{3,"nbbenthospops"},{5,"implicit_pops"},{7,"calib_oth_landings"},
        {9,"calib_weight_at_szgroup"},{11,"calib_cpue_multiplier"},{13,"int_harbours"},
        {15,"implicit_pops_level2"}, {17,"grouped_tacs"},{19,"nbcp_coupling_pops"},
    };

    if (!reader.importFromStream(stream, specs))
        return false;

    try {
        nbpops = reader.getAs<int>("nbpops");
        nbbenthospops= reader.getAs<int>("nbbenthospops");
        implicit_pops = displace::formats::utils::stringToVector<int>(reader.get("implicit_pops"), " ");
        implicit_pops_level2 = displace::formats::utils::stringToVector<int>(reader.get("implicit_pops_level2"), " ");
        grouped_tacs= displace::formats::utils::stringToVector<int>(reader.get("grouped_tacs"), " ");
        nbcp_coupling_pops= displace::formats::utils::stringToVector<int>(reader.get("nbcp_coupling_pops"), " ");
        calib_oth_landings = displace::formats::utils::stringToVector<double>(reader.get("calib_oth_landings"), " ");
        calib_w = displace::formats::utils::stringToVector<double>(reader.get("calib_weight_at_szgroup"), " ");
        calib_cpue = displace::formats::utils::stringToVector<double>(reader.get("calib_cpue_multiplier"), " ");
        interesting_harbours = types::helpers::toIdVector<types::NodeId>(displace::formats::utils::stringToVector<int>(reader.get("int_harbours"), " "));

        if(calib_oth_landings.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");
        if(calib_w.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");
        if(calib_cpue.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");

        if(grouped_tacs.empty()){
            grouped_tacs.resize(nbpops);
            iota (std::begin(grouped_tacs), std::end(grouped_tacs), 0); // Fill with 0, 1, ...,
            }
        if(nbcp_coupling_pops.empty()){
            grouped_tacs.push_back(0);
            }
        } catch (displace::formats::FormatException &x) {
#ifdef VERBOSE_ERRORS
        cerr << x.what() << endl;
#endif
        return false;
    }

#ifdef VERBOSE_ERRORS
    cout << "read config file...OK" << endl << flush;
#endif

    return true;
}


/**
read the scenario specific settings for the siums given the case study
@param the vectors to be filled in, ...
*/
bool read_scenario_config_file (string folder_name_parameterization,
                                string inputfolder,
                                string namefolderoutput,
                                displace::commons::Scenario &scenario)
{
    string filename = inputfolder+"/simusspe_"+folder_name_parameterization+"/"+namefolderoutput+".dat";
    std::cout << "Reading Scenario file from " << filename << std::endl;

    std::ifstream f (filename.c_str(), std::ios_base::in);
    return read_scenario_config_file(f,scenario);
}

bool read_scenario_config_file(std::istream &stream, displace::commons::Scenario &scenario)
{
    helpers::LineNumberReader reader;

    static const helpers::LineNumberReader::Specifications specs {
        {1,"dyn_alloc_sce"},{3,"dyn_pop_sce"},{5,"biolsce"},{7,"fleetsce"},{9,"freq_do_growth"},{11,"freq_redispatch_the_pop"},
        {13,"a_graph"},{15,"nrow_coord"},{17,"nrow_graph"},{19,"a_port"},{21,"graph_res"},
        {23,"is_individual_vessel_quotas"},{25,"check_all_stocks_before_going_fishing"},{27,"dt_go_fishing"},
        {29,"dt_choose_ground"},{31,"dt_start_fishing"},{33,"dt_change_ground"},{35,"dt_stop_fishing"},
        {37,"dt_change_port"},{39,"use_dtrees"},
        {41,"tariff_pop"},{43,"freq_update_tariff_code"},
        {45,"arbitary_breaks_for_tariff"},{47,"total_amount_credited"},{49,"tariff_annual_hcr_percent_change"},
        {51,"update_tariffs_based_on_lpue_or_dpue_code"},
        {53,"metier_closures"}
    };

    if (!reader.importFromStream(stream, specs))
        return false;

    try {
        auto dasf = reader.get("dyn_alloc_sce");
        std::vector<std::string> das;
        boost::split(das, dasf, boost::is_any_of(" "));
        for (auto d : das) {
            scenario.dyn_alloc_sce.setOption(d, true);
        }

        auto dpsf = reader.get("dyn_pop_sce");
        std::vector<std::string> dps;
        boost::split(dps, dpsf, boost::is_any_of(" "));
        for (auto d : dps) {
            scenario.dyn_pop_sce.setOption(d, true);
        }

        std::vector<int> metier_closures;
        auto met_c = reader.get("metier_closures");
        std::vector<std::string> closures;
        boost::trim(met_c);
        if (!met_c.empty()) {
            boost::split(closures, met_c, boost::is_any_of(" "));
            for (auto cl : closures) {
                metier_closures.push_back(boost::lexical_cast<int>(cl));
            }
        }
        scenario.closure_opts.setOption(Options::Closure_Opt::banned_metiers, metier_closures);

        scenario.biolsce=reader.get("biolsce");
        scenario.fleetsce=reader.get("fleetsce");
        scenario.freq_do_growth=reader.getAs<int>("freq_do_growth");
        scenario.freq_redispatch_the_pop=reader.getAs<int>("freq_redispatch_the_pop");
        scenario.a_graph=reader.getAs<int>("a_graph");
        scenario.nrow_coord=reader.getAs<int>("nrow_coord");
        scenario.nrow_graph=reader.getAs<int>("nrow_graph");
        scenario.a_port=types::NodeId(reader.getAs<int>("a_port"));
        scenario.graph_res= displace::formats::utils::stringToVector<double>(reader.get("graph_res"), " ");
        if(scenario.graph_res.size()==1) scenario.graph_res.push_back(scenario.graph_res.at(0)); //res x and y required
        scenario.is_individual_vessel_quotas= (reader.getAs<int>("is_individual_vessel_quotas") != 0);
        scenario.check_all_stocks_before_going_fishing=(reader.getAs<int>("check_all_stocks_before_going_fishing") != 0);
        scenario.dt_go_fishing=reader.get("dt_go_fishing");
        scenario.dt_choose_ground=reader.get("dt_choose_ground");
        scenario.dt_start_fishing=reader.get("dt_start_fishing");
        scenario.dt_change_ground=reader.get("dt_change_ground");
        scenario.dt_stop_fishing=reader.get("dt_stop_fishing");
        scenario.dt_change_port=reader.get("dt_change_port");
        scenario.use_dtrees=(reader.getAs<int>("use_dtrees") != 0);

        scenario.tariff_pop = displace::formats::utils::stringToVector<int>(reader.get("tariff_pop"), " ");
        scenario.freq_update_tariff_code = reader.getAs<int>("freq_update_tariff_code");
        scenario.arbitary_breaks_for_tariff = displace::formats::utils::stringToVector<double>(reader.get("arbitary_breaks_for_tariff"), " ");

        scenario.total_amount_credited = reader.getAs<int>("total_amount_credited", 0);
        scenario.tariff_annual_hcr_percent_change = reader.getAs<double>("tariff_annual_hcr_percent_change", 0);
        scenario.update_tariffs_based_on_lpue_or_dpue_code = reader.getAs<int>("update_tariffs_based_on_lpue_or_dpue_code", 0);

    } catch (displace::formats::FormatException &x) {
#ifdef VERBOSE_ERRORS
        cerr << x.what() << endl;
#endif
        return false;
    }
#ifdef VERBOSE_ERRORS
    cout << "read scenario config file...OK" <<  endl << flush;
    cout << "...e.g. graph is " << scenario.a_graph <<  endl << flush;
    cout << "...e.g. check_all_stocks_before_going_fishing is " << scenario.check_all_stocks_before_going_fishing <<  endl << flush;
#endif

    // Update the internals when needed
    scenario.closure_opts.update();

    return true;
}

vector <int> read_tsteps_quarters(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_quarters;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_quarters_2009_2015.dat";
    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename.c_str());
    }

    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_quarters.push_back(val);
        }

    }

    /*
    // check
    cout << "tsteps_quarters: " << endl;
    for(unsigned int i=0; i<tsteps_quarters.size(); i++)
    {
        cout << tsteps_quarters[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_quarters);
}


//-----------
vector <int> read_tsteps_semesters(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_semesters;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_semesters_2009_2015.dat";
    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename.c_str());
    }

    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_semesters.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_semesters: " << endl;
    for(unsigned int i=0; i<tsteps_semesters.size(); i++)
    {
        cout << tsteps_semesters[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_semesters);
}


//-----------
vector <int> read_tsteps_months(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_months;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_months_2009_2015.dat";
    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename.c_str());
    }

    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_months.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_months: " << endl;
    for(unsigned int i=0; i<tsteps_months.size(); i++)
    {
        cout << tsteps_months[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_months);
}


//-----------
vector <int> read_tsteps_years(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_years;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_years_2009_2015.dat";
    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename.c_str());
    }

    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_years.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_years: " << endl;
    for(unsigned int i=0; i<tsteps_years.size(); i++)
    {
        cout << tsteps_years[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_years);
}


//----------------
bool read_vessels_features(string a_quarter,
                           vector<string>& vesselids,
                           vector<int>& vid_is_actives,
                           vector<int>& vid_is_part_of_ref_fleets,
                           vector<double>& speeds,
                           vector<double>& fuelcons,
                           vector<double>& lengths,
                           vector<double>& vKWs,
                           vector<double>& carrycapacities,
                           vector<double>& tankcapacities,
                           vector<double>& nbfpingspertrips,
                           vector<double>& resttime_par1s,
                           vector<double>& resttime_par2s,
                           vector<double>& av_trip_duration,
                           vector<double>& mult_fuelcons_when_steaming,
                           vector<double>& mult_fuelcons_when_fishing,
                           vector<double>& mult_fuelcons_when_returning,
                           vector<double>& mult_fuelcons_when_inactive,
                           vector<int>& firm_ids,
                           string folder_name_parameterization,
                           string inputfolder,
                           int selected_vessels_only, vector<VesselCalendar> &calendars)
{

    string filename;
    if(selected_vessels_only==1)
    {
        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_features_"+a_quarter+"_subset.dat";
    }
    else
    {

        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_features_"+a_quarter+".dat";
    }
    ifstream vessels_features;
    vessels_features.open(filename.c_str());
    if(vessels_features.fail())
    {
        open_file_error(filename.c_str());
        cout << "Error when trying to open the vesselsspe_features.dat file...." << endl;
        return false;
    }

    if (!fill_from_vessels_specifications(vessels_features, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                                          speeds, fuelcons, lengths, vKWs,
                                          carrycapacities, tankcapacities, nbfpingspertrips,
                                          resttime_par1s, resttime_par2s, av_trip_duration,
                                          mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                                          mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids, calendars)) {
        vessels_features.close();
        return false;
    }

    vessels_features.close();

#ifdef VERBOSE
    // check inputs
    for (unsigned int i=0; i<speeds.size(); i++)
    {
        dout(cout << speeds[i] << " ");
    }
    dout(cout << endl);

    // check inputs
    for (unsigned int i=0; i<lengths.size(); i++)
    {
        cout << lengths[i] << " ";
    }
    cout << endl;

    // check inputs
    for (unsigned int i=0; i<vKWs.size(); i++)
    {
        cout << vKWs[i] << " ";
    }
    cout << endl;

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        dout(cout << vesselids[i] << " ");
    }
    dout(cout << endl);

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        cout << carrycapacities[i] << " ";
    }
    cout << endl;

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        dout(cout << tankcapacities[i] << " ");
    }
    dout(cout << endl);

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        dout(cout << nbfpingspertrips[i] << " ");
    }
    dout(cout << endl);

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        dout(cout << resttime_par1s[i] << " ");
    }
    dout(cout << endl);

    // check inputs
    for (unsigned int i=0; i<vesselids.size(); i++)
    {
        dout(cout << resttime_par2s[i] << " ");
    }
    dout(cout << endl);
#endif

    return true;
}


//----------------
bool read_vessels_economics_features(
                           vector<string>& vesselids,
                           vector<double>& this_vessel_nb_crews,
                           vector<double>& annual_other_incomes,
                           vector<double>& landing_costs_percents,
                           vector<double>& crewshare_and_unpaid_labour_costs_percents,
                           vector<double>& other_variable_costs_per_unit_efforts,
                           vector<double>& annual_insurance_costs_per_crews,
                           vector<double>& standard_labour_hour_opportunity_costss,
                           vector<double>& standard_annual_full_time_employement_hourss,
                           vector<double>& other_annual_fixed_costss,
                           vector<double>& vessel_values,
                           vector<double>& annual_depreciation_rates,
                           vector<double>& opportunity_interest_rates,
                           vector<double>& annual_discount_rates,
                           string folder_name_parameterization,
                           string inputfolder
                           )
{

    string filename;
    filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_economic_features.dat";

    ifstream vessels_economic_features;
    vessels_economic_features.open(filename.c_str());
    if(vessels_economic_features.fail())
    {
        open_file_error(filename.c_str());
        return false;
    }

    if (!fill_from_vessels_economic_specifications(vessels_economic_features,
                                                   this_vessel_nb_crews,
                                                   annual_other_incomes,
                                                   landing_costs_percents,
                                                   crewshare_and_unpaid_labour_costs_percents,
                                                   other_variable_costs_per_unit_efforts,
                                                   annual_insurance_costs_per_crews,
                                                   standard_labour_hour_opportunity_costss,
                                                   standard_annual_full_time_employement_hourss,
                                                   other_annual_fixed_costss,
                                                   vessel_values,
                                                   annual_depreciation_rates,
                                                   opportunity_interest_rates,
                                                   annual_discount_rates)) {
        vessels_economic_features.close();
        return false;
    }

    vessels_economic_features.close();


    return true;
}



//----------------
bool read_firms_features(vector<int>& firm_ids,
                         vector<string>& firm_names,
                         vector<int>& nb_vessels,
                         vector<double>& longs,
                         vector<double>& lats,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{

    string filename=  inputfolder+"/firmsspe_"+folder_name_parameterization+"/firms_specs.dat";

    ifstream firms_features;
    firms_features.open(filename.c_str());
    if(firms_features.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }

    bool r = fill_from_firms_specifications(firms_features, firm_ids,
                                            firm_names, nb_vessels,
                                            longs, lats);
    firms_features.close();

    return r;
}





//----------------
bool read_fishfarms_features(vector<int>& fishfarms_ids,
                         vector<string>& fishfarms_names,
                         vector<int>& idx_nodes,
                         vector<int>& is_actives,
                         vector<double>& sizes,
                         vector<double>& longs,
                         vector<double>& lats,
                         vector<double>& mean_SSTs,
                         vector<double>& mean_salinities,
                         vector<double>& mean_windspeeds,
                         vector<double>& mean_currentspeeds,
                         vector<double>& max_depths,
                         vector<double>& diss_O2_mg_per_ls,
                         vector<double>& Linf_mms,
                         vector<double>& K_ys,
                         vector<double>& t0_ys,
                         vector<double>& fulton_condition_factors,
                         vector<string>& meanw_growth_model_types,
                         vector<int>&    start_day_growings,
                         vector<int>&    end_day_harvests,
                         vector<int>&    nbyears_for_growths,
                         vector<int>&    nb_days_fallowing_periods,
                         vector<int>&    nb_fish_at_starts,
                         vector<double>& meanw_at_starts,
                         vector<double>& price_per_kg_at_starts,
                         vector<double>& target_meanw_at_harvests,
                         vector<int>& nb_fish_at_harvests,
                         vector<double>& meanw_at_harvests,
                         vector<double>& prop_harvest_kg_solds,
                         vector<double>& kg_eggs_per_kgs,
                         vector<double>& price_eggs_per_kgs,
                         vector<double>& N_in_fish_kg_3pers,
                         vector<double>& P_in_fish_kg_0_5pers,
                         vector<string>& feed_types,
                         vector<double>& feed_price_per_kgs,
                         vector<double>& total_feed_kgs,
                         vector<double>& prop_N_in_feeds,
                         vector<double>& prop_P_in_feeds,
                         vector<double>& total_feed_N_kgs,
                         vector<double>& total_feed_P_kgs,
                         vector<string>& feed_type_vets,
                         vector<double>& feed_vet_price_per_kgs,
                         vector<double>& total_feed_vet_kgs,
                         vector<double>& prop_N_in_feed_vets,
                         vector<double>& prop_P_in_feed_vets,
                         vector<double>& total_feed_vet_N_kgs,
                         vector<double>& total_feed_vet_P_kgs,
                         vector<double>& annual_discharge_N_kgs,
                         vector<double>& annual_discharge_P_kgs,
                         vector<double>& annual_discharge_C_kgs,
                         vector<double>& annual_discharge_heavymetals_kgs,
                         vector<double>& annual_discharge_medecine_kgs,
                         vector<double>& net_harvest_kg_per_sqkm_ys,
                         vector<double>& market_price_sold_fishs,
                         vector<double>& operating_cost_per_days,
                         vector<double>& annual_profits,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{


    string filename=  inputfolder+"/fishfarmsspe_"+folder_name_parameterization+"/fishfarmsspe_features.dat";

    ifstream fishfarms_features;
    fishfarms_features.open(filename.c_str());
    if(fishfarms_features.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }



    bool r = fill_from_fishfarms_specifications(fishfarms_features,
                                                fishfarms_ids,
                                                fishfarms_names,
                                                idx_nodes,
                                                is_actives,
                                                sizes,
                                                longs,
                                                lats,
                                                mean_SSTs,
                                                mean_salinities,
                                                mean_windspeeds,
                                                mean_currentspeeds,
                                                max_depths,
                                                diss_O2_mg_per_ls,
                                                Linf_mms,
                                                K_ys,
                                                t0_ys,
                                                fulton_condition_factors,
                                                meanw_growth_model_types,
                                                start_day_growings,
                                                end_day_harvests,
                                                nbyears_for_growths,
                                                nb_days_fallowing_periods,
                                                nb_fish_at_starts,
                                                meanw_at_starts,
                                                price_per_kg_at_starts,
                                                target_meanw_at_harvests,
                                                nb_fish_at_harvests,
                                                meanw_at_harvests,
                                                prop_harvest_kg_solds,
                                                kg_eggs_per_kgs,
                                                price_eggs_per_kgs,
                                                N_in_fish_kg_3pers,
                                                P_in_fish_kg_0_5pers,
                                                feed_types,
                                                feed_price_per_kgs,
                                                total_feed_kgs,
                                                prop_N_in_feeds,
                                                prop_P_in_feeds,
                                                total_feed_N_kgs,
                                                total_feed_P_kgs,
                                                feed_type_vets,
                                                feed_vet_price_per_kgs,
                                                total_feed_vet_kgs,
                                                prop_N_in_feed_vets,
                                                prop_P_in_feed_vets,
                                                total_feed_vet_N_kgs,
                                                total_feed_vet_P_kgs,
                                                annual_discharge_N_kgs,
                                                annual_discharge_P_kgs,
                                                annual_discharge_C_kgs,
                                                annual_discharge_heavymetals_kgs,
                                                annual_discharge_medecine_kgs,
                                                net_harvest_kg_per_sqkm_ys,
                                                market_price_sold_fishs,
                                                operating_cost_per_days,
                                                annual_profits
                                                );
    fishfarms_features.close();

    return r;
}




//----------------
bool read_ships_features(vector<string>& shipids,
                         vector<double> &imos,
                         vector<double> &yearbuilds, vector<string> &flags,
                         vector<string> &types, vector<double> &typecodes,
                         vector<double> &loas,
                         vector<double> &KWs,
                         vector<double> &breadths,
                         vector<double> &grosstonnages, vector<double> &nbunits,
                         vector<double> &fueluses,
                         vector<double> &NOxEmission_gperKWhs,
                         vector<double> &SOxEmission_percentpertotalfuelmasss,
                         vector<double> &GHGEmissions,
                         vector<double> &PMEmissions,
                         vector<double>& vmaxs,
                         vector<double>& vcruises,
                         vector<double>& lane_ids,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_features.dat";

    ifstream ships_features;
    ships_features.open(filename.c_str());
    if(ships_features.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }

    bool r = fill_from_ships_specifications(ships_features, shipids, imos,
                                            yearbuilds, flags, types, typecodes,
                                            loas, KWs, breadths, grosstonnages, nbunits,
                                            fueluses, NOxEmission_gperKWhs,
                                            SOxEmission_percentpertotalfuelmasss,
                                            GHGEmissions, PMEmissions,
                                            vmaxs, vcruises, lane_ids);
    ships_features.close();

    return r;
}


multimap<types::NodeId, string> read_harbour_names(string folder_name_parameterization, string inputfolder)
{

    //input data,
    string filename=  inputfolder+"/harboursspe_"+folder_name_parameterization+"/names_harbours.dat";
    ifstream harbour_names_file;
    harbour_names_file.open(filename.c_str());
    if(harbour_names_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=harbour name
    multimap<int, string> harbour_names;
    fill_multimap_from_specifications_i_s(harbour_names_file,  harbour_names);
    harbour_names_file.close();

    /*
    // check input
    multimap<int,string>::iterator lower_g = harbour_names.lower_bound(2478);
    multimap<int,string>::iterator upper_g = harbour_names.upper_bound(2478);
    cout << "for this harbour, the harbour name is: ";
    for (multimap<int, string>::iterator pos=lower_g; pos != upper_g; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;*/

    return(types::helpers::toKeyIdMultimap<types::NodeId>(harbour_names));
}


int read_prices_per_harbour(types::NodeId i, string a_quarter, multimap<string, double>& prices_per_harbour,
                            string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename=  inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_"+a_quarter+".dat";
    ifstream fish_prices_per_harbour;
    fish_prices_per_harbour.open(filename.c_str());
    if(fish_prices_per_harbour.fail())
    {
        cout << "fail to load the file for price per met per pop for this port" << endl;
        open_file_error(filename.c_str());

        return -1;
    }
    else
    {
        fill_multimap_from_specifications_s_d(fish_prices_per_harbour,  prices_per_harbour);
        fish_prices_per_harbour.close();

        // check input
        // multimap<string,double>::iterator lower3 = prices_per_harbour.lower_bound("met0");
        //multimap<string,double>::iterator upper3 = prices_per_harbour.upper_bound("met0");
        //dout << "metier-specific fish price on " << i <<": ";
        //for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
        //{
        //    dout << pos->second << " ";
        //}
        //dout << endl;

        return(0);
    }
}

void read_pop_names_in_string(map<int, string>& pop_names,
                                string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/pop_names_"+folder_name_parameterization+".txt";
    ifstream file_pop_names;
    file_pop_names.open(filename.c_str());
    if(file_pop_names.fail())
    {
        cout << "fail to read pop_names_.txt" << endl;
        open_file_error(filename.c_str());

    }
    else
    {
        fill_map_from_specifications_i_s(file_pop_names,  pop_names,  inputfolder);
        file_pop_names.close();

    }
}


void read_fuel_prices_per_vsize(types::NodeId i, string a_quarter,
                                map<int, double>& fuel_prices_per_vsize,
                                string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename = inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_"+a_quarter+"_fuel_price_per_vessel_size.dat";
    ifstream file_fuel_prices_per_vsize;
    file_fuel_prices_per_vsize.open(filename.c_str());
    if(file_fuel_prices_per_vsize.fail())
    {
        cout << "fail to load the file for fuel price per vsize for this port...search in vesselspe instead..." << endl;

       // by default:
       filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/fuel_price_per_vessel_size.dat";
       file_fuel_prices_per_vsize.open(filename.c_str());
       if(file_fuel_prices_per_vsize.fail())
       {
           cout << "fail to load the file for fuel price in vesselspe..." << endl;
           open_file_error(filename.c_str());

       }
    }


    fill_map_from_specifications_i_d(file_fuel_prices_per_vsize,  fuel_prices_per_vsize,  inputfolder);
    file_fuel_prices_per_vsize.close();



}

int read_prices_per_harbour_each_pop_per_cat(types::NodeId i, string a_quarter,
                                             multimap<int, double>& prices_per_harbour_each_species_per_cat,
                                             string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename = inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_"+a_quarter+"_each_species_per_cat.dat";
    ifstream fish_prices_per_harbour;
    fish_prices_per_harbour.open(filename.c_str());
    if(fish_prices_per_harbour.fail())
    {
        cout << "fail to load the file for price per pop per cat for this port" << endl;
        open_file_error(filename.c_str());

        return -1;
    }
    else
    {
        fill_multimap_from_specifications_i_d(fish_prices_per_harbour,  prices_per_harbour_each_species_per_cat);
        fish_prices_per_harbour.close();

        // check input
        // multimap<string,double>::iterator lower3 = prices_per_harbour.lower_bound("met0");
        //multimap<string,double>::iterator upper3 = prices_per_harbour.upper_bound("met0");
        //dout << "metier-specific fish price on " << i <<": ";
        //for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
        //{
        //    dout << pos->second << " ";
        //}
        //dout << endl;

        return(0);
    }
}


multimap<string, types::NodeId> read_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: fishing grounds
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_fgrounds_"+a_quarter+".dat";
    ifstream vessels_fgrounds;
    vessels_fgrounds.open(filename.c_str());
    if(vessels_fgrounds.fail())
    {
        open_file_error(filename.c_str());
        //   return 1;
    }
    multimap<string, int> fgrounds;
    fill_multimap_from_specifications_s_i(vessels_fgrounds,  fgrounds);
    vessels_fgrounds.close();

#ifdef VERBOSE
    // check input
    multimap<string,int>::iterator lower_fg = fgrounds.lower_bound("DNK001");
    multimap<string,int>::iterator upper_fg = fgrounds.upper_bound("DNK001");

    dout(cout << "fishing grounds: ");
    for (multimap<string, int>::iterator pos=lower_fg; pos != upper_fg; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(types::helpers::toValueIdMultimap<types::NodeId>(fgrounds));
}



multimap<string, types::NodeId> read_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: fishing grounds
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_fgrounds_"+a_quarter+".dat";
    ifstream vessels_fgrounds;
    vessels_fgrounds.open(filename.c_str());
    if(vessels_fgrounds.fail())
    {
        open_file_error(filename.c_str());
        //   return 1;
    }
    multimap<string, int> fgrounds_init;
    fill_multimap_from_specifications_s_i(vessels_fgrounds,  fgrounds_init);
    vessels_fgrounds.close();

#ifdef VERBOSE
    // check input
    multimap<string,int>::iterator lower_fg = fgrounds_init.lower_bound("DNK001");
    multimap<string,int>::iterator upper_fg = fgrounds_init.upper_bound("DNK001");

    dout(cout << "fishing grounds: ");
    for (multimap<string, int>::iterator pos=lower_fg; pos != upper_fg; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(types::helpers::toValueIdMultimap<types::NodeId>(fgrounds_init));
}



multimap<string, types::NodeId> read_harbours(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: specfic-harbour list
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_harbours_"+a_quarter+".dat";
    ifstream vessels_harbours;
    vessels_harbours.open(filename.c_str());
    if(vessels_harbours.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, int> harbours;
    fill_multimap_from_specifications_s_i(vessels_harbours,  harbours);
    vessels_harbours.close();

#ifdef VERBOSE
    // check input
    multimap<string,int>::iterator lower2 = harbours.lower_bound("DNK001");
    multimap<string,int>::iterator upper2 = harbours.upper_bound("DNK001");
    dout(cout << "specific harbours: ");
    for (multimap<string, int>::iterator pos=lower2; pos != upper2; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(types::helpers::toValueIdMultimap<types::NodeId>(harbours));
}


multimap<string, double> read_freq_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for fgrounds
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_freq_fgrounds_"+a_quarter+".dat";
    ifstream vessels_freq_fgrounds;
    vessels_freq_fgrounds.open(filename.c_str());
    if(vessels_freq_fgrounds.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> freq_fgrounds;
    fill_multimap_from_specifications_s_d(vessels_freq_fgrounds,  freq_fgrounds);
    vessels_freq_fgrounds.close();

#ifdef VERBOSE
    // check input
    multimap<string,double>::iterator lower3 = freq_fgrounds.lower_bound("DNK001");
    multimap<string,double>::iterator upper3 = freq_fgrounds.upper_bound("DNK001");
    dout(cout << "specific freq fgrounds: ");
    for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif


    return(freq_fgrounds);
}

multimap<string, double> read_freq_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for fgrounds
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_freq_fgrounds_"+a_quarter+".dat";
    ifstream vessels_freq_fgrounds;
    vessels_freq_fgrounds.open(filename.c_str());
    if(vessels_freq_fgrounds.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> freq_fgrounds_init;
    fill_multimap_from_specifications_s_d(vessels_freq_fgrounds,  freq_fgrounds_init);
    vessels_freq_fgrounds.close();

#ifdef VERBOSE
    // check input
    multimap<string,double>::iterator lower3 = freq_fgrounds_init.lower_bound("DNK001");
    multimap<string,double>::iterator upper3 = freq_fgrounds_init.upper_bound("DNK001");
    dout(cout << "specific freq fgrounds: ");
    for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif


    return(freq_fgrounds_init);
}



multimap<string, double> read_freq_harbours(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for harbours
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_freq_harbours_"+a_quarter+".dat";
    ifstream vessels_freq_harbours;
    vessels_freq_harbours.open(filename.c_str());
    if(vessels_freq_harbours.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> freq_harbours;
    fill_multimap_from_specifications_s_d(vessels_freq_harbours,  freq_harbours);
    vessels_freq_harbours.close();

#ifdef VERBOSE
    // check input
    multimap<string,double>::iterator lower4 = freq_harbours.lower_bound("DNK001");
    multimap<string,double>::iterator upper4 = freq_harbours.upper_bound("DNK001");
    dout(cout << "specific freq harbours: ");
    for (multimap<string, double>::iterator pos=lower4; pos != upper4; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(freq_harbours);
}


multimap<string, double> read_vessels_betas(string a_semester, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel catching power/skipping effect i.e. the vessel betas
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_betas_"+a_semester+".dat";
    ifstream vesselsspe_betas_file;
    vesselsspe_betas_file.open(filename.c_str());
    if(vesselsspe_betas_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> vessels_betas;
    fill_multimap_from_specifications_s_d(vesselsspe_betas_file, vessels_betas);
    vesselsspe_betas_file.close();

#ifdef VERBOSE
    // check input
    multimap<string,double>::iterator lower5 = vessels_betas.lower_bound("DNK000004561");
    multimap<string,double>::iterator upper5 = vessels_betas.upper_bound("DNK000004561");
    dout(cout << "specific catching power/skipper effect beta parameter per pop: ");
    for (multimap<string, double>::iterator pos=lower5; pos != upper5; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(vessels_betas);
}


multimap<string, double> read_vessels_tacs(string a_semester, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel catching power/skipping effect i.e. the vessel betas
    string filename;
    if(folder_name_parameterization=="final")
    {
        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_betas_"+a_semester+".dat";
    }
    else
    {
        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_percent_tacs_per_pop_"+a_semester+".dat";
    }
    ifstream vesselsspe_tacs_file;
    vesselsspe_tacs_file.open(filename.c_str());
    if(vesselsspe_tacs_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> vessels_tacs;
    fill_multimap_from_specifications_s_d(vesselsspe_tacs_file, vessels_tacs);
    vesselsspe_tacs_file.close();

    /*
    // check input
    multimap<string,double>::iterator lower5 = vessels_tacs.lower_bound("DNK000001744");
    multimap<string,double>::iterator upper5 = vessels_tacs.upper_bound("DNK000001744");
    cout << "specific tac per pop: ";
    for (multimap<string, double>::iterator pos=lower5; pos != upper5; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;
    */

    return(vessels_tacs);
}


multimap<string, double> read_initial_fishing_credits(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/initial_share_fishing_credits_per_vid.dat";

    ifstream vesselsspe_credits_file;
    vesselsspe_credits_file.open(filename.c_str());
    if(vesselsspe_credits_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<string, double> initial_fishing_credits;
    fill_multimap_from_specifications_s_d(vesselsspe_credits_file, initial_fishing_credits);
    vesselsspe_credits_file.close();

    /*
    // check input
    multimap<string,double>::iterator lower5 = initial_fishing_credits.lower_bound("DNK000001744");
    multimap<string,double>::iterator upper5 = initial_fishing_credits.upper_bound("DNK000001744");
    cout << "specific tac per pop: ";
    for (multimap<string, double>::iterator pos=lower5; pos != upper5; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;
    */

    return(initial_fishing_credits);
}




multimap<types::NodeId, int> read_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: possible metiers for this vessel
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_possible_metiers_"+a_quarter+".dat";
    ifstream vessels_possible_metiers;
    vessels_possible_metiers.open(filename.c_str());
    if(vessels_possible_metiers.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=metier name
    multimap<int, int> possible_metiers;
    fill_multimap_from_specifications_i_i(vessels_possible_metiers,  possible_metiers);
    vessels_possible_metiers.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,int>::iterator lower_g = possible_metiers.lower_bound(1600);
        multimap<int,int>::iterator upper_g = possible_metiers.upper_bound(1600);
        dout(cout << "for this vessel, possible metiers for this specific ground (1600): ");
        for (multimap<int, int>::iterator pos=lower_g; pos != upper_g; pos++)
        {
            dout(cout << pos->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(possible_metiers));
}


multimap<types::NodeId, double> read_freq_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_freq_possible_metiers_"+a_quarter+".dat";
    ifstream vessels_freq_possible_metiers;
    vessels_freq_possible_metiers.open(filename.c_str());
    if(vessels_freq_possible_metiers.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=freq
    multimap<int, double> freq_possible_metiers;
    fill_multimap_from_specifications_i_d(vessels_freq_possible_metiers,  freq_possible_metiers);
    vessels_freq_possible_metiers.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = freq_possible_metiers.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = freq_possible_metiers.upper_bound(1600);
        dout(cout << "for this vessel, frequence of possible metiers for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(freq_possible_metiers));
}


multimap<types::NodeId, double> read_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_cpue_per_stk_on_nodes_"+a_quarter+".dat";
    ifstream vessels_cpue_per_stk_on_nodes;
    vessels_cpue_per_stk_on_nodes.open(filename.c_str());
    if(vessels_cpue_per_stk_on_nodes.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=cpue kghour
    multimap<int, double> cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_cpue_per_stk_on_nodes,  cpue_per_stk_on_nodes);
    vessels_cpue_per_stk_on_nodes.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = cpue_per_stk_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = cpue_per_stk_on_nodes.upper_bound(1600);
        dout(cout << "for this vessel, frequence of possible metiers for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(cpue_per_stk_on_nodes));
}


multimap<types::NodeId, double> read_gshape_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_gshape_cpue_per_stk_on_nodes_"+a_quarter+".dat";
    ifstream vessels_gshape_cpue_per_stk_on_nodes;
    vessels_gshape_cpue_per_stk_on_nodes.open(filename.c_str());
    if(vessels_gshape_cpue_per_stk_on_nodes.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value= gamma shape param
    multimap<int, double> gshape_cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_gshape_cpue_per_stk_on_nodes,  gshape_cpue_per_stk_on_nodes);
    vessels_gshape_cpue_per_stk_on_nodes.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = gshape_cpue_per_stk_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = gshape_cpue_per_stk_on_nodes.upper_bound(1600);
        dout(cout << "for this vessel,  for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(gshape_cpue_per_stk_on_nodes));
}


multimap<types::NodeId, double> read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_gscale_cpue_per_stk_on_nodes_"+a_quarter+".dat";
    ifstream vessels_gscale_cpue_per_stk_on_nodes;
    vessels_gscale_cpue_per_stk_on_nodes.open(filename.c_str());
    if(vessels_gscale_cpue_per_stk_on_nodes.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=gamma scale param
    multimap<int, double> gscale_cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_gscale_cpue_per_stk_on_nodes,  gscale_cpue_per_stk_on_nodes);
    vessels_gscale_cpue_per_stk_on_nodes.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = gscale_cpue_per_stk_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = gscale_cpue_per_stk_on_nodes.upper_bound(1600);
        dout(cout << "for this vessel,  for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(gscale_cpue_per_stk_on_nodes));
}



multimap<types::NodeId, double> read_initial_tariffs_on_nodes(string folder_name_parameterization, string inputfolder, string a_graph_name)
{

    // obtained from looking at average cpue on nodes for the explicit species
    //string filename=  inputfolder+"/graphsspe_"+folder_name_parameterization+"/initial_tariffs_on_nodes.dat";
    string filename=  inputfolder+"/graphsspe/initial_tariffs_on_nodes_"+a_graph_name+".dat";

    ifstream initial_tariffs_on_nodes_file;
    initial_tariffs_on_nodes_file.open(filename.c_str());
    if(initial_tariffs_on_nodes_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=gamma scale param
    multimap<int, double> initial_tariffs_on_nodes;
    fill_multimap_from_specifications_i_d(initial_tariffs_on_nodes_file,  initial_tariffs_on_nodes);
    initial_tariffs_on_nodes_file.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = initial_tariffs_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = initial_tariffs_on_nodes.upper_bound(1600);
        dout(cout << "for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(initial_tariffs_on_nodes));
}






// FOR METIER
multimap<int, double> read_sel_ogives(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_selectivity_ogives.dat";

    ifstream metier_selectivity_ogives;
    metier_selectivity_ogives.open(filename.c_str());
    if(metier_selectivity_ogives.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> sel_ogives;
    fill_multimap_from_specifications_i_d(metier_selectivity_ogives,  sel_ogives);
    metier_selectivity_ogives.close();

    return(sel_ogives);
}


multimap<int, double> read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/"+a_met_s+"loss_after_one_passage_per_landscape_per_func_group.dat";

    ifstream metiers_loss_after_one_passage_per_landscape_per_func_group;
    metiers_loss_after_one_passage_per_landscape_per_func_group.open(filename.c_str());
    if(metiers_loss_after_one_passage_per_landscape_per_func_group.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> metiers_loss_after_one_passage;
    fill_multimap_from_specifications_i_d(metiers_loss_after_one_passage_per_landscape_per_func_group,  metiers_loss_after_one_passage);
    metiers_loss_after_one_passage_per_landscape_per_func_group.close();

    return(metiers_loss_after_one_passage);
}

multimap<int, double> read_logistic_recovery_rates_per_month_per_funcgr(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/logistic_recovery_rates_per_month_per_funcgr.dat";

    ifstream logistic_recovery_rates_per_month_per_funcgr;
    logistic_recovery_rates_per_month_per_funcgr.open(filename.c_str());
    if(logistic_recovery_rates_per_month_per_funcgr.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> rates_per_month_per_funcgr;
    fill_multimap_from_specifications_i_d(logistic_recovery_rates_per_month_per_funcgr,  rates_per_month_per_funcgr);
    logistic_recovery_rates_per_month_per_funcgr.close();

    return(rates_per_month_per_funcgr);
}


multimap<int, double> read_habitat_deltas_per_pop(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/habitat_deltas_per_pop.dat";

    ifstream habitat_deltas_per_pop_file;
    habitat_deltas_per_pop_file.open(filename.c_str());
    if(habitat_deltas_per_pop_file.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> habitat_deltas_per_pop;
    fill_multimap_from_specifications_i_d(habitat_deltas_per_pop_file,  habitat_deltas_per_pop);
    habitat_deltas_per_pop_file.close();

    return(habitat_deltas_per_pop);
}



multimap<int, double> read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.dat";

    ifstream benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
    benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.open(filename.c_str());
    if(benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> K_per_landscape_per_funcgr;
    fill_multimap_from_specifications_i_d(benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,  K_per_landscape_per_funcgr);
    benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.close();

    return(K_per_landscape_per_funcgr);
}


multimap<int, double> read_benthos_number_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/benthos_number_carrying_capacity_K_per_landscape_per_funcgr.dat";

    ifstream benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
    benthos_number_carrying_capacity_K_per_landscape_per_funcgr.open(filename.c_str());
    if(benthos_number_carrying_capacity_K_per_landscape_per_funcgr.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> K_per_landscape_per_funcgr;
    fill_multimap_from_specifications_i_d(benthos_number_carrying_capacity_K_per_landscape_per_funcgr,  K_per_landscape_per_funcgr);
    benthos_number_carrying_capacity_K_per_landscape_per_funcgr.close();

    return(K_per_landscape_per_funcgr);
}


multimap<int, int> read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/met_target_names.dat";

    ifstream file_metier_target_stocks;
    file_metier_target_stocks.open(filename.c_str());
    if(file_metier_target_stocks.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, int> metier_target_stocks;
    fill_multimap_from_specifications_i_i(file_metier_target_stocks,  metier_target_stocks);
    file_metier_target_stocks.close();

    return(metier_target_stocks);
}

multimap<int, int> read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_suitable_seabottomtypes.dat";

    ifstream file_metier_suitable_seabottomtypes;
    file_metier_suitable_seabottomtypes.open(filename.c_str());
    if(file_metier_suitable_seabottomtypes.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, int> metier_suitable_seabottomtypes;
    fill_multimap_from_specifications_i_i(file_metier_suitable_seabottomtypes,  metier_suitable_seabottomtypes);
    file_metier_suitable_seabottomtypes.close();

    return(metier_suitable_seabottomtypes);
}


// FOR METIER
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/combined_met_types.dat";

    ifstream metier_types;
    metier_types.open(filename.c_str());
    if(metier_types.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, int> met_types;
    fill_map_from_specifications_i_i (metier_types,  met_types, folder_name_parameterization);
    metier_types.close();

    return(met_types);
}



// FOR METIER
map<int, double> read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/percent_revenue_completenesses.dat";

    ifstream metier_percent_revenue_completenesses;
    metier_percent_revenue_completenesses.open(filename.c_str());
    if(metier_percent_revenue_completenesses.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, double> percent_revenue_completenesses;
    fill_map_from_specifications_i_d (metier_percent_revenue_completenesses,  percent_revenue_completenesses, folder_name_parameterization);
    metier_percent_revenue_completenesses.close();

    return(percent_revenue_completenesses);
}


// FOR METIER
map<int, double> read_metiers_fspeed(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_fspeed.dat";

    ifstream metier_fspeed;
    metier_fspeed.open(filename.c_str());
    if(metier_fspeed.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, double> fspeed;
    fill_map_from_specifications_i_d (metier_fspeed,  fspeed, folder_name_parameterization);
    metier_fspeed.close();

    return(fspeed);
}


// FOR METIER
map<int, double> read_gear_widths_param_a(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_param_a.dat";

    ifstream metier_gear_widths_param_a;
    metier_gear_widths_param_a.open(filename.c_str());
    if(metier_gear_widths_param_a.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }

    //cout << "here:" << filename << endl;

    map<int, double> gear_widths_param_a;
    fill_map_from_specifications_i_d (metier_gear_widths_param_a,  gear_widths_param_a, folder_name_parameterization);
    metier_gear_widths_param_a.close();

    //cout << "here:" << gear_widths_param_a[0] << endl;
    //cout << "here:" << gear_widths_param_a[1] << endl;
    //=> do not forget the heading in input files given first line is skipped



    return(gear_widths_param_a);
}


// FOR METIER
map<int, double> read_gear_widths_param_b(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_param_b.dat";

    ifstream metier_gear_widths_param_b;
    metier_gear_widths_param_b.open(filename.c_str());
    if(metier_gear_widths_param_b.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, double> gear_widths_param_b;
    fill_map_from_specifications_i_d (metier_gear_widths_param_b,  gear_widths_param_b, folder_name_parameterization);
    metier_gear_widths_param_b.close();

    return(gear_widths_param_b);
}


// FOR METIER
map<int, string> read_gear_widths_model_type(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_model_type.dat";

    ifstream metier_gear_widths_model_type;
    metier_gear_widths_model_type.open(filename.c_str());
    if(metier_gear_widths_model_type.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, string> gear_widths_model_type;
    fill_map_from_specifications_i_s (metier_gear_widths_model_type,  gear_widths_model_type, folder_name_parameterization);
    metier_gear_widths_model_type.close();

    return(gear_widths_model_type);
}


// FOR METIER
multimap<int, double> read_dis_ogives(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_discards_ogives.dat";

    ifstream metier_discards_ogives;
    metier_discards_ogives.open(filename.c_str());
    if(metier_discards_ogives.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> dis_ogives;
    fill_multimap_from_specifications_i_d(metier_discards_ogives,  dis_ogives);
    metier_discards_ogives.close();

    return(dis_ogives);
}



multimap<int, double> read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_betas_"+a_semester+".dat";

    ifstream metierspe_betas_file;
    metierspe_betas_file.open(filename.c_str());
    if(metierspe_betas_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> metiers_betas;
    fill_multimap_from_specifications_i_d(metierspe_betas_file,  metiers_betas);
    metierspe_betas_file.close();

    return(metiers_betas);
}

multimap<int, double> read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_discardratio_limits_"+a_semester+".dat";

    ifstream discardratio_limits_file;
    discardratio_limits_file.open(filename.c_str());
    if(discardratio_limits_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> discardratio_limits;
    fill_multimap_from_specifications_i_d(discardratio_limits_file,  discardratio_limits);
    discardratio_limits_file.close();

    return(discardratio_limits);
}


multimap<int, int> read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_is_avoided_stocks_"+a_semester+".dat";

    ifstream is_avoided_stocks_file;
    is_avoided_stocks_file.open(filename.c_str());
    if(is_avoided_stocks_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, int> is_avoided_stocks;
    fill_multimap_from_specifications_i_i(is_avoided_stocks_file,  is_avoided_stocks);
    is_avoided_stocks_file.close();

    return(is_avoided_stocks);
}



multimap<int, int> read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_mls_cat_"+a_semester+".dat";

    ifstream metierspe_mls_cat_file;
    metierspe_mls_cat_file.open(filename.c_str());
    if(metierspe_mls_cat_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, int> metierspe_mls_cat;
    fill_multimap_from_specifications_i_i(metierspe_mls_cat_file,  metierspe_mls_cat);
    metierspe_mls_cat_file.close();

    return(metierspe_mls_cat);
}


// FOR BENTHOS
multimap<int, double> read_prop_funcgr_biomass_per_node_per_landscape(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/prop_funcgr_biomass_per_node_per_landscape.dat";

    ifstream file_prop_funcgr_per_node;
    file_prop_funcgr_per_node.open(filename.c_str());
    if(file_prop_funcgr_per_node.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> prop_funcgr_per_node;
    fill_multimap_from_specifications_i_d(file_prop_funcgr_per_node,  prop_funcgr_per_node);
    file_prop_funcgr_per_node.close();

    return(prop_funcgr_per_node);
}


// FOR BENTHOS
multimap<int, double> read_longevity_classes_condition_per_node(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/longevity_classes_condition_per_node.dat";

    ifstream file_longevity_classes_condition_per_node;
    file_longevity_classes_condition_per_node.open(filename.c_str());
    if(file_longevity_classes_condition_per_node.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> longevity_classes_condition_per_node;
    fill_multimap_from_specifications_i_d(file_longevity_classes_condition_per_node,  longevity_classes_condition_per_node);
    file_longevity_classes_condition_per_node.close();

    return(longevity_classes_condition_per_node);
}


multimap<int, double> read_prop_funcgr_number_per_node_per_landscape(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/prop_funcgr_number_per_node_per_landscape.dat";

    ifstream file_prop_funcgr_per_node;
    file_prop_funcgr_per_node.open(filename.c_str());
    if(file_prop_funcgr_per_node.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> prop_funcgr_per_node;
    fill_multimap_from_specifications_i_d(file_prop_funcgr_per_node,  prop_funcgr_per_node);
    file_prop_funcgr_per_node.close();

    return(prop_funcgr_per_node);
}

multimap<int, double> read_meanw_funcgr_per_landscape(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/benthosspe_"+folder_name_parameterization+"/meanw_funcgr_per_landscape.dat";

    ifstream file_meanw_funcgr_per_node;
    file_meanw_funcgr_per_node.open(filename.c_str());
    if(file_meanw_funcgr_per_node.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> meanw_funcgr_per_node;
    fill_multimap_from_specifications_i_d(file_meanw_funcgr_per_node,  meanw_funcgr_per_node);
    file_meanw_funcgr_per_node.close();

    return(meanw_funcgr_per_node);
}


// FOR FISHFARMS
map<int, double> read_size_per_farm(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/fishfarmsspe_"+folder_name_parameterization+"/size_per_farm.dat";

    ifstream file_size_per_farm;
    file_size_per_farm.open(filename.c_str());
    if(file_size_per_farm.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, double> size_per_farm;
    fill_map_from_specifications(file_size_per_farm,  size_per_farm);
    file_size_per_farm.close();

    return(size_per_farm);
}

// FOR WINDMILLS
map<int, double> read_size_per_windmill(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/windmillsspe_"+folder_name_parameterization+"/size_per_windmill.dat";

    ifstream file_size_per_windmill;
    file_size_per_windmill.open(filename.c_str());
    if(file_size_per_windmill.fail())
    {
        open_file_error(filename.c_str());
        //return 1;
    }
    map<int, double> size_per_windmill;
    fill_map_from_specifications(file_size_per_windmill,  size_per_windmill);
    file_size_per_windmill.close();

    return(size_per_windmill);
}

// FOR SHIPPING LANES
multimap<int, double> read_shiplanes_lat(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_lanes_lat.dat";

    ifstream file_shipsspe_lanes_lat;
    file_shipsspe_lanes_lat.open(filename.c_str());
    if(file_shipsspe_lanes_lat.fail())
    {
        cout << "Unfortunately the /shipsspe_lanes_lat.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> shipsspe_lanes_lat;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lat,  shipsspe_lanes_lat);
    file_shipsspe_lanes_lat.close();

    return(shipsspe_lanes_lat);
}



multimap<int, double> read_shiplanes_lon(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_lanes_lon.dat";

    ifstream file_shipsspe_lanes_lon;
    file_shipsspe_lanes_lon.open(filename.c_str());
    if(file_shipsspe_lanes_lon.fail())
    {
        cout << "Unfortunately the /shipsspe_lanes_lon.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        //return 1;
    }
    multimap<int, double> shipsspe_lanes_lon;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lon,  shipsspe_lanes_lon);
    file_shipsspe_lanes_lon.close();

    return(shipsspe_lanes_lon);
}





// FOR POPULATION
multimap<int, double> read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/avai"+szgroup+"_betas_"+a_semester+".dat";

    //input data, pop characteristics: e.g. avai0_betas i.e. betas per pop for szgroup 0
    ifstream avai_betas_file;
    avai_betas_file.open(filename.c_str());
    if(avai_betas_file.fail())
    {
        cout << "Unfortunately the /avaiXX_betas_semesterXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        //return 1;
    }
    // it is actually only a map...but was simpler to reuse the multimap routines
    multimap<int, double> avai_betas;
    fill_multimap_from_specifications_i_d(avai_betas_file,  avai_betas);
    avai_betas_file.close();

    return(avai_betas);
}


multimap<int, double> read_init_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_pops_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: initial N
    ifstream file_init_pops_per_szgroup;
    file_init_pops_per_szgroup.open(filename.c_str());
    if(file_init_pops_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_pops_per_szgroup_biolsce1.dat";
        file_init_pops_per_szgroup.open(filename.c_str());
    }
    if(file_init_pops_per_szgroup.fail())
    {
        cout << "Unfortunately the init_pops_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_pops_per_szgroup,  init_pops_per_szgroup);
    file_init_pops_per_szgroup.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = init_pops_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init = init_pops_per_szgroup.upper_bound(0);
    dout(cout << "initial N at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_pops_per_szgroup);
}



multimap<int, double> read_adults_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/_adults_diet_preference_per_stock_allstks_biolsce"+biolsce+".dat";

    //input data
    ifstream file_adults_diet_preference_per_stock_allstks;
    file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    if(file_adults_diet_preference_per_stock_allstks.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/adults_diet_preference_per_stock_allstks_biolsce1.dat";
        file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if(file_adults_diet_preference_per_stock_allstks.fail())
    {
        cout << "Unfortunately the adults_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> adults_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_adults_diet_preference_per_stock_allstks,  adults_diet_preference_per_stock_allstks);
    file_adults_diet_preference_per_stock_allstks.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = adults_diet_preference_per_stock_allstks.lower_bound(0);
    multimap<int,double>::iterator upper_init = adults_diet_preference_per_stock_allstks.upper_bound(0);
    dout(cout << "adults_diet_preference_per_stock_allstks for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(adults_diet_preference_per_stock_allstks);
}


multimap<int, double> read_juveniles_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/_juveniles_diet_preference_per_stock_allstks_biolsce"+biolsce+".dat";

    //input data
    ifstream file_juveniles_diet_preference_per_stock_allstks;
    file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    if(file_juveniles_diet_preference_per_stock_allstks.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/juveniles_diet_preference_per_stock_allstks_biolsce1.dat";
        file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if(file_juveniles_diet_preference_per_stock_allstks.fail())
    {
        cout << "Unfortunately the juveniles_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> juveniles_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_juveniles_diet_preference_per_stock_allstks,  juveniles_diet_preference_per_stock_allstks);
    file_juveniles_diet_preference_per_stock_allstks.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = juveniles_diet_preference_per_stock_allstks.lower_bound(0);
    multimap<int,double>::iterator upper_init = juveniles_diet_preference_per_stock_allstks.upper_bound(0);
    dout(cout << "juveniles_diet_preference_per_stock_allstks for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(juveniles_diet_preference_per_stock_allstks);
}



multimap<int, double> read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_prop_migrants_pops_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: initial prop migrants
    ifstream file_init_prop_migrants_pops_per_szgroup;
    file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    if(file_init_prop_migrants_pops_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_prop_migrants_pops_per_szgroup_biolsce1.dat";
        file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    }
    if(file_init_prop_migrants_pops_per_szgroup.fail())
    {
        cout << "Unfortunately the init_prop_migrants_pops_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_prop_migrants_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_prop_migrants_pops_per_szgroup,  init_prop_migrants_pops_per_szgroup);
    file_init_prop_migrants_pops_per_szgroup.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = init_prop_migrants_pops_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init = init_prop_migrants_pops_per_szgroup.upper_bound(0);
    dout(cout << "initial N at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_prop_migrants_pops_per_szgroup);
}







multimap<int, double> read_init_maturity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_maturity_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_maturity_per_szgroup;
    file_init_maturity_per_szgroup.open(filename.c_str());
    if(file_init_maturity_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_maturity_per_szgroup_biolsce1.dat";
        file_init_maturity_per_szgroup.open(filename.c_str());
    }
    if(file_init_maturity_per_szgroup.fail())
    {
        cout << "Unfortunately the init_maturity_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_maturity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_maturity_per_szgroup,  init_maturity_per_szgroup);
    file_init_maturity_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_mat = init_maturity_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_mat = init_maturity_per_szgroup.upper_bound(0);
    dout(cout << "initial maturity at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_mat; pos != upper_init_mat; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_maturity_per_szgroup);
}


multimap<int, double> read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_fecundity_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_fecundity_per_szgroup;
    file_init_fecundity_per_szgroup.open(filename.c_str());
    if(file_init_fecundity_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_fecundity_per_szgroup_biolsce1.dat";
        file_init_fecundity_per_szgroup.open(filename.c_str());
    }
    if(file_init_fecundity_per_szgroup.fail())
    {
        cout << "Unfortunately the init_fecundity_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_fecundity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_fecundity_per_szgroup,  init_fecundity_per_szgroup);
    file_init_fecundity_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_fec = init_fecundity_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_fec = init_fecundity_per_szgroup.upper_bound(0);
    dout(cout << "initial fecundity at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_fec; pos != upper_init_fec; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_fecundity_per_szgroup);
}


multimap<int, double> read_init_weight_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_weight_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_weight_per_szgroup;
    file_init_weight_per_szgroup.open(filename.c_str());
    if(file_init_weight_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_weight_per_szgroup_biolsce1.dat";
        file_init_weight_per_szgroup.open(filename.c_str());
    }
    if(file_init_weight_per_szgroup.fail())
    {
        cout << "Unfortunately the init_weight_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_weight_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_weight_per_szgroup,  init_weight_per_szgroup);
    file_init_weight_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_we = init_weight_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_we = init_weight_per_szgroup.upper_bound(0);
    dout(cout << "initial weight at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_we; pos != upper_init_we; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_weight_per_szgroup);
}


multimap<int, int> read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/comcat_per_szgroup_done_by_hand.dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_comcat_per_szgroup;
    file_comcat_per_szgroup.open(filename.c_str());
    if(file_comcat_per_szgroup.fail())
    {
        cout << "Unfortunately the comcat_per_szgroup_done_by_hand.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, int> comcat_per_szgroup;
    fill_multimap_from_specifications_i_i(file_comcat_per_szgroup,  comcat_per_szgroup);
    file_comcat_per_szgroup.close();

    /*
    // check input
    multimap<int,int>::iterator lower_init_we = comcat_per_szgroup.lower_bound(0);
    multimap<int,int>::iterator upper_init_we = comcat_per_szgroup.upper_bound(0);
    cout << "comcat at szgroup for pop0: ";
    for (multimap<int, int>::iterator pos=lower_init_we; pos != upper_init_we; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;
    */

    return(comcat_per_szgroup);
}


multimap<int, double> read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_M_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_M_per_szgroup;
    file_init_M_per_szgroup.open(filename.c_str());
    if(file_init_M_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_M_per_szgroup_biolsce1.dat";
        file_init_M_per_szgroup.open(filename.c_str());
    }
    if(file_init_M_per_szgroup.fail())
    {
        cout << "Unfortunately the init_M_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_M_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_M_per_szgroup,  init_M_per_szgroup);
    file_init_M_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_M = init_M_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_M = init_M_per_szgroup.upper_bound(0);
    dout(cout << "initial M at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos=lower_init_M; pos != upper_init_M; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_M_per_szgroup);
}


multimap<int, double> read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_proprecru_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_proprecru_per_szgroup;
    file_init_proprecru_per_szgroup.open(filename.c_str());
    if(file_init_proprecru_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_proprecru_per_szgroup_biolsce1.dat";
        file_init_proprecru_per_szgroup.open(filename.c_str());
    }
    if(file_init_proprecru_per_szgroup.fail())
    {
        cout << "Unfortunately the init_proprecru_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, double> init_proprecru_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_proprecru_per_szgroup,  init_proprecru_per_szgroup);
    file_init_proprecru_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_proprecru = init_proprecru_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_proprecru = init_proprecru_per_szgroup.upper_bound(0);
    dout(cout << "initial proprecru at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos=lower_init_proprecru; pos != upper_init_proprecru; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(init_proprecru_per_szgroup);
}


multimap<int, types::NodeId> read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder, string str_rand_avai_file)
{

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/"+"lst_idx_nodes_per_pop_"+a_semester+".dat";
    }
    else
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/"+"lst_idx_nodes_per_pop_"+a_semester+".dat";
        // filename=  "popsspe_"+folder_name_parameterization+"/stochast_avai/"+"lst_idx_nodes_per_pop_"+a_semester+"_"+str_rand_avai_file+".dat";
    }

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_lst_idx_nodes_per_pop;
    file_lst_idx_nodes_per_pop.open(filename.c_str());
    if(file_lst_idx_nodes_per_pop.fail())
    {
        cout << "Unfortunately the lst_idx_nodes_per_pop_.dat vector is not informed  "<< endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, int> lst_idx_nodes_per_pop;
    fill_multimap_from_specifications_i_i(file_lst_idx_nodes_per_pop,  lst_idx_nodes_per_pop);
    file_lst_idx_nodes_per_pop.close();

#ifdef VERBOSE
    // check input
    multimap<int,int>::iterator lower_idx = lst_idx_nodes_per_pop.lower_bound(0);
    multimap<int,int>::iterator upper_idx = lst_idx_nodes_per_pop.upper_bound(0);
    dout(cout << "lst idx nodes for pop 0: ");
    for (multimap<int, int>::iterator pos=lower_idx; pos != upper_idx; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    // TODO (fba#5#): remove possible replicates in the list of nodes per pop

    return(types::helpers::toValueIdMultimap<types::NodeId>(lst_idx_nodes_per_pop));
}


multimap<int, int> read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder)
{

    string filename;
    filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/"+"the_selected_szgroups.dat";

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_the_selected_szgroups;
    file_the_selected_szgroups.open(filename.c_str());
    if(file_the_selected_szgroups.fail())
    {
        cout << "Unfortunately the the_selected_szgroups.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    multimap<int, int> the_selected_szgroups;
    fill_multimap_from_specifications_i_i(file_the_selected_szgroups,  the_selected_szgroups);
    file_the_selected_szgroups.close();

    /*
    // check input
    multimap<int,int>::iterator lower_idx = the_selected_szgroups.lower_bound(0);
    multimap<int,int>::iterator upper_idx = the_selected_szgroups.upper_bound(0);
    cout << "the_selected_szgroups for pop 0: ";
    for (multimap<int, int>::iterator pos=lower_idx; pos != upper_idx; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;*/

    return(the_selected_szgroups);
}


map<int, int> read_tac_percent_simulated(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/percent_landings_from_simulated_vessels.dat";

    //input data, pop characteristics: percent_landings
    ifstream file_tac_percent_simulated;
    file_tac_percent_simulated.open(filename.c_str());
    if(file_tac_percent_simulated.fail())
    {
        cout << "Unfortunately the percent_landings_from_simulated_vessels.dat vector is not informed" << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    map<int, int> tac_percent_simulated;
    fill_map_from_specifications_i_i(file_tac_percent_simulated,  tac_percent_simulated);
    file_tac_percent_simulated.close();

#ifdef VERBOSE
    // check input
    for ( map<int,int>::iterator it=tac_percent_simulated.begin() ; it != tac_percent_simulated.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << endl);
#endif

    return(tac_percent_simulated);
}


map<int, double> read_hyperstability_param(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/hyperstability_param.dat";

    //input data, pop characteristics: hyperstability
    ifstream file_hyperstability_param;
    file_hyperstability_param.open(filename.c_str());
    if(file_hyperstability_param.fail())
    {
        cout << "Unfortunately the hyperstability_param.dat vector is not informed "<< endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename.c_str());
        // return 1;
    }
    map<int, double> hyperstability_param;
    fill_map_from_specifications_i_d(file_hyperstability_param,  hyperstability_param, folder_name_parameterization);
    file_hyperstability_param.close();

#ifdef VERBOSE
    // check input
    for ( map<int,double>::iterator it=hyperstability_param.begin() ; it != hyperstability_param.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << endl);
#endif

    return(hyperstability_param);
}



map<types::NodeId, double> read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization, string inputfolder, string fleetsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if(fleetsce=="")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_stecf_oth_land_per_month_per_node_"+a_semester+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_stecf_oth_land_per_month_per_node_"+a_month+"_fleetsce"+fleetsce+".dat";
    }

    ifstream file_oth_land;
    file_oth_land.open(filename.c_str());
    if(file_oth_land.fail())
    {
        cout << "Unfortunately the spe_stecf_oth_land_per_month_per_node_ vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename);
        //return 1;
    }
    map<types::NodeId, double> oth_land;
    fill_from_oth_land (file_oth_land, oth_land);
    file_oth_land.close();

#ifdef VERBOSE
    // check input
    map<int,double>::iterator pos;
    dout(cout << " oth_land " << endl);
    for (pos=oth_land.begin(); pos != oth_land.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(oth_land);
}





multimap<int, double> read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/"+a_pop_s+"overall_migration_fluxes_"+a_semester+"_biolsce"+biolsce+".dat";

    ifstream file_overall_migration_fluxes;
    file_overall_migration_fluxes.open(filename.c_str());
    if(file_overall_migration_fluxes.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/"+a_pop_s+"overall_migration_fluxes_"+a_semester+"_biolsce1.dat";
        file_overall_migration_fluxes.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_overall_migration_fluxes.fail())
    {
        cout << "Unfortunately the overall_migration_fluxes_ vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        open_file_error(filename);
        //return 1;
    }
    multimap<int, double> overall_migration_fluxes;
    fill_from_overall_migration_fluxes (file_overall_migration_fluxes, overall_migration_fluxes);
    file_overall_migration_fluxes.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator pos;
    dout(cout << " overall_migration_fluxes " << endl);
    for (pos=overall_migration_fluxes.begin(); pos != overall_migration_fluxes.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(overall_migration_fluxes);
}





map<string, double> read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if(folder_name_parameterization=="final")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" +
                a_pop_s + "ctrysspe_relative_stability_"+a_semester+".dat";
    }
    else
    {
        //=> NEW_VERSION: replaced by:
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" +
                a_pop_s + "ctrysspe_relative_stability_"+a_semester+".dat";
    }

    ifstream file_relative_stability;
    file_relative_stability.open(filename.c_str());
    if(file_relative_stability.fail())
    {
        open_file_error(filename);
        //return 1;
    }
    map<string, double> relative_stability;
    fill_from_relative_stability (file_relative_stability, relative_stability);
    file_relative_stability.close();

#ifdef VERBOSE
    // check input
    map<string,double>::iterator pos;
    dout(cout << " relative_stability " << endl);
    for (pos=relative_stability.begin(); pos != relative_stability.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(relative_stability);
}


multimap<types::NodeId, double> read_avai_szgroup_nodes_with_pop(string a_semester,
                                                                 int a_pop, string folder_name_parameterization, string inputfolder,
                                                                 string str_rand_avai_file,
                                                                 string type_of_avai_field_to_read)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    //cout << "type_of_avai_field_to_read is " << type_of_avai_field_to_read << endl;

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" +
                       a_pop_s + "spe_avai_szgroup_nodes_"+a_semester+type_of_avai_field_to_read+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/stochast_avai/" +
                       a_pop_s + "spe_avai_szgroup_nodes_"+a_semester+type_of_avai_field_to_read+".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        cout << "Unfortunately the availability vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        cout << "This could happen if the stock is missing in the displace_input_for_data_merger_xx.dat along the avai_updater_on Option" << endl;
        cout << "this might also result from an attempt to use an empty vmslikefpingsonly.dat file (empty if no ref. vessels defined)" << endl;
        open_file_error(filename);
        //return 1;
    }
    multimap<types::NodeId, double> avai_szgroup_nodes_with_pop;
    fill_from_avai_szgroup_nodes_with_pop (file_avai_szgroup_nodes_with_pop, avai_szgroup_nodes_with_pop);
    file_avai_szgroup_nodes_with_pop.close();

#ifdef VERBOSE
    // check input
    //node 1600
    multimap<int,double>::iterator lower = avai_szgroup_nodes_with_pop.lower_bound(1600);
    multimap<int,double>::iterator upper = avai_szgroup_nodes_with_pop.upper_bound(1600);
    dout(cout << "avai on node 1600: ");
    for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return(avai_szgroup_nodes_with_pop);
}


multimap<types::NodeId, double> read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop,
                                                                      string folder_name_parameterization, string inputfolder,
                                                                      string str_rand_avai_file,
                                                                      string type_of_avai_field_to_read)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" +
                a_pop_s + "spe_full_avai_szgroup_nodes_"+a_semester+type_of_avai_field_to_read+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/stochast_avai/" +
                a_pop_s + "spe_full_avai_szgroup_nodes_"+a_semester+"_"+str_rand_avai_file+type_of_avai_field_to_read+".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        open_file_error(filename);
        //return 1;
    }
    dout(cout << "filename for pop  " << a_pop << " is " << filename << endl;)

    multimap<types::NodeId, double> full_avai_szgroup_nodes_with_pop;
    if (!fill_from_avai_szgroup_nodes_with_pop (file_avai_szgroup_nodes_with_pop, full_avai_szgroup_nodes_with_pop))
        throw std::runtime_error("Error while executing: fill_from_avai_szgroup_nodes_with_pop");

    file_avai_szgroup_nodes_with_pop.close();

    // check input
    //multimap<int,double>::iterator lower = full_avai_szgroup_nodes_with_pop.lower_bound(1600); //node 1600
    //multimap<int,double>::iterator upper = full_avai_szgroup_nodes_with_pop.upper_bound(1600);
    //dout << "avai on node 1600: ";
    //for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
    //{
    //    dout << pos->second << " ";
    //}
    //dout << endl;
    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return(full_avai_szgroup_nodes_with_pop);
}



multimap<types::NodeId, double> read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop,
                                                                       string folder_name_parameterization, string inputfolder,
                                                                       string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_field_of_coeff_diffusion_this_pop_nodes_"+a_semester+"_biolsce"+biolsce+".dat";

    ifstream file_field_of_coeff_diffusion_this_pop;
    file_field_of_coeff_diffusion_this_pop.open(filename.c_str());
    if(file_field_of_coeff_diffusion_this_pop.fail())
    {
        open_file_error(filename);
        //return 1;
    }
    multimap<types::NodeId, double> field_of_coeff_diffusion_this_pop;
    if (!fill_field_of_coeff_diffusion_this_pop (file_field_of_coeff_diffusion_this_pop, field_of_coeff_diffusion_this_pop))
        throw std::runtime_error("Error while executing: fill_field_of_coeff_diffusion_this_pop");

    file_field_of_coeff_diffusion_this_pop.close();


    return(field_of_coeff_diffusion_this_pop);
}




vector< vector<double> > read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup,int nbage, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_szgroup_per_age_biolsce"+biolsce+".dat";

    ifstream file_percent_szgroup_per_age_matrix;
    file_percent_szgroup_per_age_matrix.open(filename.c_str());
    if(file_percent_szgroup_per_age_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_szgroup_per_age_biolsce1.dat";
        file_percent_szgroup_per_age_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_percent_szgroup_per_age_matrix.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > percent_szgroup_per_age_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_szgroup_per_age_matrix(file_percent_szgroup_per_age_matrix, percent_szgroup_per_age_matrix))
        throw std::runtime_error("Error while executuing: fill_in_percent_szgroup_per_age_matrix");

    file_percent_szgroup_per_age_matrix.close();

    return(percent_szgroup_per_age_matrix);
}


vector< vector<double> > read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup,int nbage, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_age_per_szgroup_biolsce"+biolsce+".dat";

    ifstream file_percent_age_per_szgroup_matrix;
    file_percent_age_per_szgroup_matrix.open(filename.c_str());
    if(file_percent_age_per_szgroup_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_age_per_szgroup_biolsce1.dat";
        file_percent_age_per_szgroup_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_percent_age_per_szgroup_matrix.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > percent_age_per_szgroup_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_age_per_szgroup_matrix(file_percent_age_per_szgroup_matrix, percent_age_per_szgroup_matrix))
        throw std::runtime_error("Error while executuing: fill_in_percent_age_per_szgroup_matrix");

    file_percent_age_per_szgroup_matrix.close();

    return(percent_age_per_szgroup_matrix);
}


vector< vector<double> > read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_size_transition_matrix_biolsce"+biolsce+".dat";

    ifstream file_size_transition_matrix;
    file_size_transition_matrix.open(filename.c_str());
    if(file_size_transition_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_size_transition_matrix_biolsce1.dat";
        file_size_transition_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_size_transition_matrix.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > growth_transition_matrix(nbszgroup, vector<double>(nbszgroup));
    if (!fill_in_growth_transition(file_size_transition_matrix, growth_transition_matrix))
        throw std::runtime_error("Error while executuing: fill_in_growth_trransition_matrix");

    file_size_transition_matrix.close();

    return(growth_transition_matrix);
}


vector< vector<double> > read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "preferences_for_species_matrix_biolsce"+biolsce+".dat";

    ifstream file_preferences_for_species;
    file_preferences_for_species.open(filename.c_str());
    if(file_preferences_for_species.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "preferences_for_species_matrix_biolsce1.dat";
        file_preferences_for_species.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_preferences_for_species.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > preferences_for_species_matrix(nbpops, vector<double>(nbszgroup));
    if (!fill_in_preferences_for_species_matrix(file_preferences_for_species, preferences_for_species_matrix))
        throw std::runtime_error("Error while executing: fill_in_preferences_for_species_matrix");

    file_preferences_for_species.close();

    return(preferences_for_species_matrix);
}



vector< vector<double> > read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/species_interactions_mortality_proportion_matrix_biolsce"+biolsce+".dat";

    ifstream file_species_interactions;
    file_species_interactions.open(filename.c_str());
    if(file_species_interactions.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/species_interactions_mortality_proportion_matrix_biolsce1.dat";
        file_species_interactions.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_species_interactions.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > species_interactions_mortality_proportion_matrix(nbpops, vector<double>(nbpops));
    if (!fill_in_species_interactions_mortality_proportion_matrix(file_species_interactions, species_interactions_mortality_proportion_matrix))
        throw std::runtime_error("Error while executuing: fill_in_species_interactions_mortality_proportion_matrix");

    file_species_interactions.close();

    return(species_interactions_mortality_proportion_matrix);
}



vector< vector<double> > read_selectivity_per_stock_ogives(int a_met,
                                                           int nbpops,
                                                           int nbszgroup,
                                                           string folder_name_parameterization,
                                                           string inputfolder,
                                                           string fleetsce)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename = inputfolder+"/metiersspe_"+folder_name_parameterization+"/" + a_met_s + "metier_selectivity_per_stock_ogives_fleetsce"+fleetsce+".dat";

    ifstream file_selectivity_per_stock_ogives;
    file_selectivity_per_stock_ogives.open(filename.c_str());
    if(file_selectivity_per_stock_ogives.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector< vector<double> > selectivity_per_stock_ogives(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives, selectivity_per_stock_ogives))
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");

    file_selectivity_per_stock_ogives.close();

    return(selectivity_per_stock_ogives);
}

vector< vector<double> > read_selectivity_per_stock_ogives_for_oth_land(int nbpops,
                                                                        int nbszgroup,
                                                                        string folder_name_parameterization,
                                                                        string inputfolder,
                                                                        string fleetsce)
{

    string filename = inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_selectivity_per_stock_ogives_fleetsce"+fleetsce+"_for_oth_land.dat";

    ifstream file_selectivity_per_stock_ogives_for_oth_land;
    file_selectivity_per_stock_ogives_for_oth_land.open(filename.c_str());
    if(file_selectivity_per_stock_ogives_for_oth_land.fail())
    {
        vector<vector <double> > selectivity_per_stock_ogives_for_oth_land;
         return selectivity_per_stock_ogives_for_oth_land; // caution: returns an empty object
    }
    vector< vector<double> > selectivity_per_stock_ogives_for_oth_land(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives_for_oth_land, selectivity_per_stock_ogives_for_oth_land))
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");

    file_selectivity_per_stock_ogives_for_oth_land.close();

    return(selectivity_per_stock_ogives_for_oth_land);
}




vector<double>  read_param_sr(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_SSB_R_parameters_biolsce"+biolsce+".dat";

    ifstream file_param_sr;
    file_param_sr.open(filename.c_str());
    if(file_param_sr.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_SSB_R_parameters_biolsce1.dat";
        file_param_sr.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_param_sr.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector<double> param_sr(3);
    if (!fill_in_param_sr(file_param_sr, param_sr))
        throw std::runtime_error("Error while executuing: fill_in_param_sr");

    file_param_sr.close();

    return(param_sr);
}


vector<double>  read_initial_tac(int a_pop,  string folder_name_parameterization,  string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_initial_tac.dat";

    ifstream file_initial_tac;
    file_initial_tac.open(filename.c_str());
    if(file_initial_tac.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    vector<double> initial_tac(1);
    if (!fill_in_initial_tac(file_initial_tac, initial_tac))
        throw std::runtime_error("Error while executuing: fill_in_initial_tac");
    file_initial_tac.close();

    return(initial_tac);
}


vector<double>  read_fbar_ages_min_max_and_ftarget(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent_biolsce"+biolsce+".dat";

    ifstream file_fbar_ages_min_max;
    file_fbar_ages_min_max.open(filename.c_str());
    if(file_fbar_ages_min_max.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent.dat";
        file_fbar_ages_min_max.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_fbar_ages_min_max.fail())
    {
        open_file_error(filename.c_str());
    }
    // i.e. 8 columns: F min age, F max age, LTMP ftarget, Fpercent, TACpercent, Btrigger, F-MSY value, F-MSY-low, F-MSY-up
    vector<double> fbar_ages_min_max(9);
    if (!fill_in_fbar_ages_min_max(file_fbar_ages_min_max, fbar_ages_min_max))
        throw std::runtime_error("bad file format while executing: fill_in_fbar_ages_min_max");
    file_fbar_ages_min_max.close();


    return(fbar_ages_min_max);
}


spp::sparse_hash_map<types::NodeId::type, types::NodeId::type> read_maps_previous(types::NodeId source, string namesimu,  string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read map previous" << endl);

    stringstream out;
    out << source.toIndex();
    string source_s = out.str();
    string filename= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/previous_"+source_s+".bin";

    bool r;
    spp::sparse_hash_map<types::NodeId::type, types::NodeId::type> previous;		 //key, value
    try {
        displace::formats::legacy::BinaryGraphFileReader rdr;
        r = rdr.importFromStream<uint16_t,uint16_t>(filename, [&previous](uint16_t key, uint16_t value) {
            previous.insert(std::pair<int,int>(key,value));
            return true;
        });
    } catch (std::exception &x) {
        cerr << "Exception : " << x.what() << endl;
        r = false;
    }

    if (!r) {
        cerr << "Error loading graph file " << filename << " " << strerror(errno) << endl;
        throw std::runtime_error("Error loading graph file " + filename + " " + strerror(errno)) ;
    }
    //dout(cout <<"END: read map previous" << endl);

    return previous;
}

spp::sparse_hash_map<types::NodeId::type, int> read_min_distance(types::NodeId source, string namesimu, string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read min_distance" << endl);

    stringstream out;
    out << source.toIndex();
    string source_s = out.str();
    string filename= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/min_distance_"+source_s+".bin";

    bool r;
    spp::sparse_hash_map<types::NodeId::type, int> min_distance;		 //key, value
    try {
        displace::formats::legacy::BinaryGraphFileReader rdr;
        r = rdr.importFromStream<uint16_t,uint16_t>(filename, [&min_distance](uint16_t key, uint16_t value) {
            min_distance.insert(std::pair<int,int>(key,value));
            return true;
        });
    } catch (std::exception &x) {
        cerr << "Exception : " << x.what() << endl;
        r = false;
    }

    if (!r) {
        cerr << "Error loading graph file " << filename << " " << strerror(errno) << endl;
        throw std::runtime_error("Error loading graph file " + filename + " " + strerror(errno)) ;
    }


    //dout(cout <<"END: read min_distance" << endl);

    return min_distance;
}

PathShop read_graph_details(types::NodeId source, string namesimu,  string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read map previous" << endl);

    stringstream out1;
    out1 << source.toIndex();
    string source_s1 = out1.str();
    string filename_previous= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/previous_"+source_s1+".bin";

    //dout(cout <<"BEGIN: read min_distance" << endl);

    stringstream out2;
    out2 << source.toIndex();
    string source_s2 = out2.str();
    string filename_weight= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/min_distance_"+source_s2+".bin";

    try{
     return   PathShop::readFromFiles(filename_previous, filename_weight);
    //    return a_shop;
    } catch (std::exception &x) {
        cout << " for node " << source_s1 << "," << endl;
        cerr << " Error in reading graphsspe\\shortPaths_ .bin files : " << x.what() << endl;
        throw;
    }
}

multimap<int, int> read_nodes_in_polygons(string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    string filename = inputfolder+"/graphsspe/nodes_in_polygons_"+a_graph+"_"+a_quarter+".dat";

    ifstream file_nodes_in_polygons;
    file_nodes_in_polygons.open(filename.c_str());
    if(file_nodes_in_polygons.fail())
    {
        open_file_error(filename);
        //return 1;
    }
    multimap<int, int> nodes_in_polygons;
    fill_from_nodes_in_polygons (file_nodes_in_polygons, nodes_in_polygons);
    file_nodes_in_polygons.close();

    // check input
    //multimap<int,int>::iterator lower = nodes_in_polygons.lower_bound();
    //multimap<int,int>::iterator upper = nodes_in_polygons.upper_bound();
    //dout << " nodes_in_polygons " << endl;
    //for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
    //{
    //    dout << pos->second << " ";
    //}
    //dout << endl;

    return(nodes_in_polygons);
}

bool read_metier_quarterly_closures (vector <Node*> &nodes, string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/metier_closure_"+a_graph+"_"+a_quarter+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_metier_closures(is, separator, banning);

    if (r) {
        for (auto &info : banning) {
            int count =0;
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedMetier(id);
                nodes.at(info.nodeId.toIndex())->setNbOfDaysClosedPerMonth(id, info.nbOfDaysClosedPerMonth.at(count));
                ++count;
            }

        }
    }

    is.close();
    return r;
}

bool read_metier_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/metier_closure_"+a_graph+"_"+a_month+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_metier_closures(is, separator, banning);
    if (r) {
        for (auto &info : banning) {
            int count =0;
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedMetier(id);
                nodes.at(info.nodeId.toIndex())->setNbOfDaysClosedPerMonth(id, info.nbOfDaysClosedPerMonth.at(count));
                ++count;
            }

        }
    }


    is.close();
    return r;
}

bool read_metier_closures(istream &stream, const std::string &separator, vector<NodeBanningInfo> &nodes)
{
    // Format:
    // PolyId nbOfDaysClosed NodeId Metier [Metier[ Metier...]]

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));
            NodeBanningInfo info;
            info.nodeId = types::NodeId(boost::lexical_cast<int>(sr[2]));
            for (size_t i = 3; i < sr.size(); ++i) {
                int m = boost::lexical_cast<int>(sr[i]);
                info.banned.push_back(m);
                info.nbOfDaysClosedPerMonth.push_back(boost::lexical_cast<double>(sr[1]));
            }
            nodes.push_back(info);
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
//#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on read_metier_closures file line " << linenum <<
                " : " << ex.what() << "\n";
//#endif
        return false;
    }

    return true;
}

bool read_vsize_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/vsize_closure_"+a_graph+"_"+a_month+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_vsize_closures(is, separator, banning);

    if (r) {
        for (auto &info : banning) {
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedVsize(id);
                nodes.at(info.nodeId.toIndex())->setAreaType(1);
            }
        }
    }

    is.close();
    return r;
}

bool read_vsize_closures(istream &stream, const std::string &separator, vector<NodeBanningInfo> &nodes)
{
    // Format:
    // PolyId nbOfDaysClosed NodeId Vessel Size [Vessel Size[ Vessel Size...]]

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));


            NodeBanningInfo info;
            info.nodeId = types::NodeId(boost::lexical_cast<int>(sr[2]));
            for (size_t i = 3; i < sr.size(); ++i) {
                int m = boost::lexical_cast<int>(sr[i]);
                info.banned.push_back(m);
            }
            nodes.push_back(info);
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
//#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on read_vsize_closure file line " << linenum <<
                " : " << ex.what() << "\n";
//#endif
        return false;
    }

    return true;
}



bool read_biological_traits_params(istream &stream, const std::string &separator, std::vector <std::tuple< string, double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, string> >  & biological_traits_params)
{
    // Format:
    //0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34
    //stock	Winf	k	Linf	K	t0	a	b	L50	alpha	beta	r_age	tac_tons	fbar_age_min	fbar_age_max	F_target	F_percent	TAC_percent	B_trigger	FMSY	fbar_assessment	ssb_assessment	mls_cat	mls	size_bin_cm	unit_sizebin	CV_recru	mat	mat_cat	etha_m	kappa	q	n	fzeroest	species

    // TODO: FOR NOW, more or less ONLY Winf k ARE USED....BUT THINK ABOUT REPLACING INPUTS FOR OTHER PARAMS FROM HERE.

    cout << "Reading biological_traits_params..." << endl;

    std::string dummystring;
    getline (stream, dummystring); // eat the heading

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));

            std::tuple< string, double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, string>  a_tuple;

            std::get<0>(a_tuple)=boost::lexical_cast<string>(sr[0]);
            std::get<1>(a_tuple)=boost::lexical_cast<double>(sr[1]);
            std::get<2>(a_tuple)=boost::lexical_cast<double>(sr[2]);
            std::get<3>(a_tuple)=boost::lexical_cast<double>(sr[3]);
            std::get<4>(a_tuple)=boost::lexical_cast<double>(sr[4]);
            std::get<5>(a_tuple)=boost::lexical_cast<double>(sr[5]);
            std::get<6>(a_tuple)=boost::lexical_cast<double>(sr[6]);
            std::get<7>(a_tuple)=boost::lexical_cast<double>(sr[7]);
            std::get<8>(a_tuple)=boost::lexical_cast<double>(sr[8]);
            std::get<9>(a_tuple)=boost::lexical_cast<double>(sr[9]);
            std::get<10>(a_tuple)=boost::lexical_cast<double>(sr[10]);
            std::get<11>(a_tuple)=boost::lexical_cast<double>(sr[11]);
            std::get<12>(a_tuple)=boost::lexical_cast<double>(sr[12]);
            std::get<13>(a_tuple)=boost::lexical_cast<double>(sr[13]);
            std::get<14>(a_tuple)=boost::lexical_cast<double>(sr[14]);
            std::get<15>(a_tuple)=boost::lexical_cast<double>(sr[15]);
            std::get<16>(a_tuple)=boost::lexical_cast<double>(sr[16]);
            std::get<17>(a_tuple)=boost::lexical_cast<double>(sr[17]);
            std::get<18>(a_tuple)=boost::lexical_cast<double>(sr[18]);
            std::get<19>(a_tuple)=boost::lexical_cast<double>(sr[19]);
            std::get<20>(a_tuple)=boost::lexical_cast<double>(sr[20]);
            std::get<21>(a_tuple)=boost::lexical_cast<double>(sr[21]);
            std::get<22>(a_tuple)=boost::lexical_cast<double>(sr[22]);
            std::get<23>(a_tuple)=boost::lexical_cast<double>(sr[23]);
            std::get<24>(a_tuple)=boost::lexical_cast<double>(sr[24]);
            std::get<25>(a_tuple)=boost::lexical_cast<double>(sr[25]);
            std::get<26>(a_tuple)=boost::lexical_cast<double>(sr[26]);
            std::get<27>(a_tuple)=boost::lexical_cast<double>(sr[27]);
            std::get<28>(a_tuple)=boost::lexical_cast<double>(sr[28]);
            std::get<29>(a_tuple)=boost::lexical_cast<double>(sr[29]);
            std::get<30>(a_tuple)=boost::lexical_cast<double>(sr[30]);
            std::get<31>(a_tuple)=boost::lexical_cast<double>(sr[31]);
            std::get<32>(a_tuple)=boost::lexical_cast<double>(sr[32]);
            std::get<33>(a_tuple)=boost::lexical_cast<double>(sr[33]);
            std::get<34>(a_tuple)=boost::lexical_cast<string>(sr[34]);

            // check
            //cout << "0: " << std::get<0>(a_tuple) << endl;
            //cout << "1: " << std::get<1>(a_tuple) << endl;
            //cout << "2: " << std::get<2>(a_tuple) << endl;

            // TO DO: other params...
            biological_traits_params.push_back(a_tuple);


            ++linenum;
        }
        cout << "Reading biological_traits_params...ok" << endl;

    } catch (boost::bad_lexical_cast &ex) {
        cerr << "Bad Conversion on biological_traits_params file line " << linenum <<
                " : " << ex.what() << "\n";
        return false;
    }

    return true;
}


bool read_environment_on_coord(istream &stream, const std::string &separator, std::vector <EnvironmentDataRecord> & environment_on_coord)
{
    // Format:
    //"x"                     "y"                     "harb"                  "pt_graph"              "code_area"             "landscapes_code"       "landscape_norm"
    //"landscape_alpha"       "sst"                   "sst_norm"              "sst_alpha"             "salinity"              "salinity_norm"         "salinity_alpha"        "nitrogen"
    //"nitrogen_norm"         "nitrogen_alpha"        "phosphorus"            "phosphorus_norm"       "phosphorus_alpha"      "oxygen"                "oxygen_norm"
    //"oxygen_alpha"          "dissolvedcarbon"       "dissolvedcarbon_norm"  "dissolvedcarbon_alpha" "bathymetry" "shippingdensity" "siltfraction"

    cout << "Reading environment_on_coord..." << endl;

    std::string dummystring;
    getline (stream, dummystring); // eat the heading

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));

            EnvironmentDataRecord a_tuple;

            a_tuple.x=boost::lexical_cast<double>(sr[0]);
            a_tuple.y=boost::lexical_cast<double>(sr[1]);
            a_tuple.harb=boost::lexical_cast<int>(sr[2]);
            a_tuple.pt_graph=boost::lexical_cast<int>(sr[3]);
            a_tuple.code_area=boost::lexical_cast<int>(sr[4]);
            a_tuple.landscapes_code=boost::lexical_cast<int>(sr[5]);
            a_tuple.landscape_norm=boost::lexical_cast<double>(sr[6]);
            a_tuple.landscape_alpha=boost::lexical_cast<double>(sr[7]);
            a_tuple.wind = boost::lexical_cast<double>(sr[8]);
            a_tuple.wind_norm = boost::lexical_cast<double>(sr[9]);
            a_tuple.wind_alpha = boost::lexical_cast<double>(sr[10]);
            a_tuple.sst=boost::lexical_cast<double>(sr[11]);
            a_tuple.sst_norm=boost::lexical_cast<double>(sr[12]);
            a_tuple.sst_alpha=boost::lexical_cast<double>(sr[13]);
            a_tuple.salinity=boost::lexical_cast<double>(sr[14]);
            a_tuple.salinity_norm=boost::lexical_cast<double>(sr[15]);
            a_tuple.salinity_alpha=boost::lexical_cast<double>(sr[16]);
            a_tuple.nitrogen=boost::lexical_cast<double>(sr[17]);
            a_tuple.nitrogen_norm=boost::lexical_cast<double>(sr[18]);
            a_tuple.nitrogen_alpha=boost::lexical_cast<double>(sr[19]);
            a_tuple.phosphorus=boost::lexical_cast<double>(sr[20]);
            a_tuple.phosphorus_norm=boost::lexical_cast<double>(sr[21]);
            a_tuple.phosphorus_alpha=boost::lexical_cast<double>(sr[22]);
            a_tuple.oxygen=boost::lexical_cast<double>(sr[23]);
            a_tuple.oxygen_norm=boost::lexical_cast<double>(sr[24]);
            a_tuple.oxygen_alpha=boost::lexical_cast<double>(sr[25]);
            a_tuple.dissolvedcarbon=boost::lexical_cast<double>(sr[26]);
            a_tuple.dissolvedcarbon_norm=boost::lexical_cast<double>(sr[27]);
            a_tuple.dissolvedcarbon_alpha= boost::lexical_cast<double>(sr[28]);
            a_tuple.bathymetry=boost::lexical_cast<double>(sr[29]);
            a_tuple.shippingdensity=boost::lexical_cast<double>(sr[30]);
            a_tuple.siltfraction=boost::lexical_cast<double>(sr[31]);

            // check
            //cout << "reading  environment_on_coord: " << endl;
            //cout << "0: " << std::get<0>(a_tuple) << endl;
            //cout << "1: " << std::get<1>(a_tuple) << endl;
            //cout << "2: " << std::get<2>(a_tuple) << endl;

            // TO DO: other params...
            environment_on_coord.push_back(a_tuple);


            ++linenum;
        }
        cout << "Reading environment_on_coord...ok" << endl;

    } catch (boost::bad_lexical_cast &ex) {
        cerr << "Bad Conversion on environment_on_coord file line " << linenum <<
                " : " << ex.what() << "\n";
        return false;
    }

    return true;
}



void write_SMS_OP_N_in_file(ofstream& SMS_N_in,
                            vector<Population* >& populations,
                            vector<int> stock_numbers,
                            vector<int> some_units,
                            vector<int> some_max_nb_ages)
{

    // SMS-OM needs two inputs for forecasting Ny, which are the initial N i.e. Ny-1 and F applied during y-1
    // furthermore SMS needs quartely-based Fs

    SMS_N_in << setprecision(2) << fixed;
    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector< vector<double> > percent_szgroup_per_age_matrix(NBSZGROUP, vector<double>(NBAGE));
        percent_szgroup_per_age_matrix=populations.at( stock_numbers.at(i) )->get_percent_szgroup_per_age_matrix();

        // convert N from size to age
        vector<double> tot_N_at_szgroup=populations.at( stock_numbers.at(i) )->get_tot_N_at_szgroup();
        vector<double> tot_N_at_age(some_max_nb_ages.at(i));
        dout(cout << "-- st" << stock_numbers.at(i)  << endl);
        for(unsigned int sz=0; sz<tot_N_at_szgroup.size(); sz++)
        {
            for(int a=0; a<some_max_nb_ages.at(i); a++)
            {
                tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * tot_N_at_szgroup[sz] ;
                dout(cout << tot_N_at_age[a] << "= " << percent_szgroup_per_age_matrix[sz][a] << " * " << tot_N_at_szgroup[sz] << endl);
            }
        }

        // write N.in SMS files
        dout(cout << "write down the SMS n.in file...");

        for(unsigned int a = 0; a < tot_N_at_age.size(); a++)
        {
            SMS_N_in  << tot_N_at_age.at(a) / some_units.at(i) << " " ;
        }
        SMS_N_in << endl;

    }
    dout(cout << "OK...\n");

}


void write_SMS_OP_F_in_file(ofstream& SMS_F_in,
                            vector<Population* >& populations,
                            vector<int> stock_numbers)
{

    SMS_F_in << "# quarter: xx "<< endl;
    int nb_ages_in_sms=8;		 // MAGIC NUMBER

    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector <double> tot_F_at_age;
        vector <double> tot_F_at_age_last_quarter;
        // obtain the quartely Fs
        // at this stage, this is a cumul...
        tot_F_at_age              = populations.at(stock_numbers.at(i))->get_tot_F_at_age();
        tot_F_at_age_last_quarter = populations.at(stock_numbers.at(i))->get_tot_F_at_age_last_quarter();

        // get rid of the cumul with a diff with last quarter
        for(int a=0; a<nb_ages_in_sms; a++)
        {
            tot_F_at_age[a]=tot_F_at_age[a] - tot_F_at_age_last_quarter[a];
        }

        // write F.in SMS files
        dout(cout << "write down the SMS f.in file...");

        for(int a = 0; a < nb_ages_in_sms; a++)
        {
            SMS_F_in  << tot_F_at_age.at(a) << " " ;
        }
        SMS_F_in << endl;
    }

}


void read_SMS_OP_N_out_file(vector<Population* >& populations,
                            vector<int> stock_numbers,
                            vector<int> some_units,
                            vector<int> some_max_nb_ages,
                            string sms_folder,
                            string namesimu)
{
    UNUSED(some_max_nb_ages);

    // read the input file
    string filename=  sms_folder+"/"+namesimu+"/op_n.out";


    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename);
        //return 1;
    }

    // cod
    vector<double> a_vector_line2;
    // herring
    vector<double> a_vector_line3;
    // sprat
    vector<double> a_vector_line4;

    string line;
    int counter=0;
    while(!getline(in, line).eof())
    {
        std::stringstream linestream(line);
        counter+=1;
        if(counter==2)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line2.push_back(val*some_units.at(0));
            }
        }

        if(counter==3)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line3.push_back(val*some_units.at(1));
            }
        }
        if(counter==4)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line4.push_back(val*some_units.at(2));
            }
        }
    }
    dout(cout << "read SMS op_n.out...OK" << endl << flush);

    // caution: SMS OP provides 8 age classes while DISPLACE deals with NBAGE (e.g. 11 age classes)
    // so, adjustment needed:
    while (a_vector_line2.size()<NBAGE)
    {
        a_vector_line2.push_back(0);
    }
    while (a_vector_line3.size()<NBAGE)
    {
        a_vector_line3.push_back(0);
    }
    while (a_vector_line4.size()<NBAGE)
    {
        a_vector_line4.push_back(0);
    }
    dout(cout << "make nb of age classes consistent...OK" << endl << flush);

    // assign to the population objects
    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector<double> tot_N_at_age;
        if(i==0) tot_N_at_age=a_vector_line2 ; // cod - 10
        if(i==1) tot_N_at_age=a_vector_line3 ; // herring - 3
        if(i==2) tot_N_at_age=a_vector_line4 ; // sprat - 7

        populations.at( stock_numbers.at(i) )->set_tot_N_at_age(tot_N_at_age);

        vector< vector<double> > percent_age_per_szgroup_matrix(NBSZGROUP, vector<double>(NBAGE));
        percent_age_per_szgroup_matrix=populations.at( stock_numbers.at(i) )->get_percent_age_per_szgroup_matrix();

        // convert back to size classes
        // (also accounting for the units.)
        vector<double>N_at_szgroup(NBSZGROUP);
        for(unsigned int a=0; a<tot_N_at_age.size(); a++)
        {
            for(unsigned int sz=0; sz<N_at_szgroup.size(); sz++)
            {
                N_at_szgroup[sz]+=percent_age_per_szgroup_matrix[sz][a] *tot_N_at_age[a];

                /*
                if(i==0)
                {
                    dout(cout << "age" << a << " sz" << sz << endl);
                    dout(cout << N_at_szgroup[sz] << "+= " << percent_age_per_szgroup_matrix[sz][a] << " * " << tot_N_at_age[a] << endl);
                }*/
            }
        }

        // ...and fill in with the predicted N
        populations.at(stock_numbers.at(i))->set_tot_N_at_szgroup(N_at_szgroup);

    }
    dout(cout << "set Ns at szgroup from the SMS outcomes...OK" << endl << flush);

}





string getLastErrorMessage()
{
    return error_msg;
}
