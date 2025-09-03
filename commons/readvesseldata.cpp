//
// Created by happycactus on 3/1/20.
//

#include "readvesseldata.h"
#include "idtypeshelpers.h"
#include "readdata.h"

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
                           int selected_vessels_only, vector<VesselCalendar> &calendars)
{

    string filename;
    if (selected_vessels_only == 1) {
        filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_features_quarter" +
                   a_quarter + "_subset.dat";
    } else {

        filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_features_quarter" +
                   a_quarter + ".dat";
    }
    ifstream vessels_features;
    vessels_features.open(filename.c_str());
    if (vessels_features.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }

    if (!fill_from_vessels_specifications(vessels_features, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                                          speeds, fuelcons, lengths, vKWs,
                                          carrycapacities, tankcapacities, nbfpingspertrips,
                                          resttime_par1s, resttime_par2s, av_trip_duration,
                                          mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                                          mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids,
                                          calendars)) {
        vessels_features.close();
        return false;
    }

    vessels_features.close();


    return true;
}


//----------------
bool read_vessels_economics_features(
        vector<string> &vesselids,
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
)
{

    string filename;
    filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_economic_features.dat";

    ifstream vessels_economic_features;
    vessels_economic_features.open(filename.c_str());
    if (vessels_economic_features.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);

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


multimap<string, types::NodeId> read_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: fishing grounds
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_fgrounds_quarter" + a_quarter +
            ".dat";
    ifstream vessels_fgrounds;
    vessels_fgrounds.open(filename.c_str());
    if (vessels_fgrounds.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, int> fgrounds;
    fill_multimap_from_specifications_s_i(vessels_fgrounds, fgrounds);
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
    dout(cout << "\n");
#endif

    return (types::helpers::toValueIdMultimap<types::NodeId>(fgrounds));
}


multimap<string, types::NodeId>
read_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: fishing grounds
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_fgrounds_quarter" + a_quarter +
            ".dat";
    ifstream vessels_fgrounds;
    vessels_fgrounds.open(filename.c_str());
    if (vessels_fgrounds.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, int> fgrounds_init;
    fill_multimap_from_specifications_s_i(vessels_fgrounds, fgrounds_init);
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
    dout(cout << "\n");
#endif

    return (types::helpers::toValueIdMultimap<types::NodeId>(fgrounds_init));
}


multimap<string, types::NodeId> read_harbours(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: specfic-harbour list
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_harbours_quarter" + a_quarter +
            ".dat";
    ifstream vessels_harbours;
    vessels_harbours.open(filename.c_str());
    if (vessels_harbours.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, int> harbours;
    fill_multimap_from_specifications_s_i(vessels_harbours, harbours);
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
    dout(cout << "\n");
#endif

    return (types::helpers::toValueIdMultimap<types::NodeId>(harbours));
}


multimap<string, double> read_freq_fgrounds(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for fgrounds
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_freq_fgrounds_quarter" +
            a_quarter + ".dat";
    ifstream vessels_freq_fgrounds;
    vessels_freq_fgrounds.open(filename.c_str());
    if (vessels_freq_fgrounds.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, double> freq_fgrounds;
    fill_multimap_from_specifications_s_d(vessels_freq_fgrounds, freq_fgrounds);
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
    dout(cout << "\n");
#endif


    return (freq_fgrounds);
}

multimap<string, double>
read_freq_fgrounds_init(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for fgrounds
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_freq_fgrounds_quarter" +
            a_quarter + ".dat";
    ifstream vessels_freq_fgrounds;
    vessels_freq_fgrounds.open(filename.c_str());
    if (vessels_freq_fgrounds.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, double> freq_fgrounds_init;
    fill_multimap_from_specifications_s_d(vessels_freq_fgrounds, freq_fgrounds_init);
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
    dout(cout << "\n");
#endif


    return (freq_fgrounds_init);
}


multimap<string, double> read_freq_harbours(string a_quarter, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: frequency for harbours
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_freq_harbours_quarter" +
            a_quarter + ".dat";
    ifstream vessels_freq_harbours;
    vessels_freq_harbours.open(filename.c_str());
    if (vessels_freq_harbours.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<string, double> freq_harbours;
    fill_multimap_from_specifications_s_d(vessels_freq_harbours, freq_harbours);
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
    dout(cout << "\n");
#endif

    return (freq_harbours);
}


multimap<string, double> read_vessels_betas(string a_semester, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel catching power/skipping effect i.e. the vessel betas
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_betas_semester" + a_semester +
            ".dat";
    ifstream vesselsspe_betas_file;
    vesselsspe_betas_file.open(filename.c_str());
    if (vesselsspe_betas_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
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
    dout(cout << "\n");
#endif

    return (vessels_betas);
}


multimap<string, double> read_vessels_tacs(string a_semester, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel catching power/skipping effect i.e. the vessel betas
    string filename;
    if (folder_name_parameterization == "final") {
        filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_betas_semester" +
                   a_semester + ".dat";
    } else {
        filename = inputfolder + "/vesselsspe_" + folder_name_parameterization +
                   "/vesselsspe_percent_tacs_per_pop_semester" + a_semester + ".dat";
    }
    ifstream vesselsspe_tacs_file;
    vesselsspe_tacs_file.open(filename.c_str());
    if (vesselsspe_tacs_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
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
    cout << "\n";
    */

    return (vessels_tacs);
}


multimap<string, double> read_initial_fishing_credits(string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/initial_share_fishing_credits_per_vid.dat";

    ifstream vesselsspe_credits_file;
    vesselsspe_credits_file.open(filename.c_str());
    if (vesselsspe_credits_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
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
    cout << "\n";
    */

    return (initial_fishing_credits);
}


multimap<types::NodeId, int>
read_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    //input data, vessel characteristics: possible metiers for this vessel
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/" + a_vessel + "_possible_metiers_quarter" +
            a_quarter + ".dat";
    ifstream vessels_possible_metiers;
    vessels_possible_metiers.open(filename.c_str());
    if (vessels_possible_metiers.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value=metier name
    multimap<int, int> possible_metiers;
    fill_multimap_from_specifications_i_i(vessels_possible_metiers, possible_metiers);
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
        dout(cout << "\n");
    }
#endif

    return (types::helpers::toKeyIdMultimap<types::NodeId>(possible_metiers));
}


multimap<types::NodeId, double>
read_freq_possible_metiers(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/" + a_vessel +
                      "_freq_possible_metiers_quarter" + a_quarter + ".dat";
    ifstream vessels_freq_possible_metiers;
    vessels_freq_possible_metiers.open(filename.c_str());
    if (vessels_freq_possible_metiers.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value=freq
    multimap<int, double> freq_possible_metiers;
    fill_multimap_from_specifications_i_d(vessels_freq_possible_metiers, freq_possible_metiers);
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
        dout(cout << "\n");
    }
#endif

    return (types::helpers::toKeyIdMultimap<types::NodeId>(freq_possible_metiers));
}


multimap<types::NodeId, double>
read_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/" + a_vessel +
                      "_cpue_per_stk_on_nodes_quarter" + a_quarter + ".dat";
    ifstream vessels_cpue_per_stk_on_nodes;
    vessels_cpue_per_stk_on_nodes.open(filename.c_str());
    if (vessels_cpue_per_stk_on_nodes.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value=cpue kghour
    multimap<int, double> cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_cpue_per_stk_on_nodes, cpue_per_stk_on_nodes);
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
        dout(cout << "\n");
    }
#endif

    return (types::helpers::toKeyIdMultimap<types::NodeId>(cpue_per_stk_on_nodes));
}


multimap<types::NodeId, double>
read_gshape_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization,
                                  string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/" + a_vessel +
                      "_gshape_cpue_per_stk_on_nodes_quarter" + a_quarter + ".dat";
    ifstream vessels_gshape_cpue_per_stk_on_nodes;
    vessels_gshape_cpue_per_stk_on_nodes.open(filename.c_str());
    if (vessels_gshape_cpue_per_stk_on_nodes.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value= gamma shape param
    multimap<int, double> gshape_cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_gshape_cpue_per_stk_on_nodes, gshape_cpue_per_stk_on_nodes);
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
        dout(cout << "\n");
    }
#endif

    return (types::helpers::toKeyIdMultimap<types::NodeId>(gshape_cpue_per_stk_on_nodes));
}


multimap<types::NodeId, double>
read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization,
                                  string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename = inputfolder + "/vesselsspe_" + folder_name_parameterization + "/" + a_vessel +
                      "_gscale_cpue_per_stk_on_nodes_quarter" + a_quarter + ".dat";
    ifstream vessels_gscale_cpue_per_stk_on_nodes;
    vessels_gscale_cpue_per_stk_on_nodes.open(filename.c_str());
    if (vessels_gscale_cpue_per_stk_on_nodes.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value=gamma scale param
    multimap<int, double> gscale_cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_gscale_cpue_per_stk_on_nodes, gscale_cpue_per_stk_on_nodes);
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
        dout(cout << "\n");
    }
#endif

    return (types::helpers::toKeyIdMultimap<types::NodeId>(gscale_cpue_per_stk_on_nodes));
}



