//
// Created by fuga on 29/02/2020.
//

#include "commons_global.h"

#include "TextfileModelLoader.h"
#include "Impl.h"
#include "Vessel.h"
#include "myutils.h"
#include "Calendar.h"
#include "readvesseldata.h"
#include "readdata.h"

#include <map>
#include <vector>


using namespace std;

ofstream  Q_DECL_EXPORT export_individual_tacs;

namespace {
struct VesselsData {
    std::vector<string> vectsparam1;
    std::vector<string> vectsparam2;
    std::vector<string> vectsparam3;
    std::vector<string> vectsparam4;
    std::vector<string> vectsparam5;
    std::vector<VesselCalendar> vectcalendar1;
    std::vector<double> vectdparam1;
    std::vector<double> vectdparam2;
    std::vector<double> vectdparam3;
    std::vector<double> vectdparam4;
    std::vector<double> vectdparam5;
    std::vector<double> vectdparam6;
    std::vector<double> vectdparam7;
    std::vector<double> vectdparam8;
    std::vector<double> vectdparam9;
    std::vector<double> vectdparam10;
    std::vector<double> vectdparam11;
    std::vector<double> vectdparam12;
    std::vector<double> vectdparam13;
    std::vector<double> vectdparam14;
    std::vector<double> vectdparam15;
    std::vector<double> vectdparam16;
    std::vector<double> vectdparam17;
    std::vector<double> vectdparam18;
    std::vector<double> vectdparam19;
    std::vector<double> vectdparam20;
    std::vector<double> vectdparam21;
    std::vector<double> vectdparam22;
    std::vector<double> vectdparam23;
    std::vector<double> vectdparam24;
    std::vector<double> vectdparam25;
    std::vector<double> vectdparam26;
    std::vector<double> vectdparam27;
    std::vector<double> vectdparam28;
    std::vector<double> vectdparam29;
    std::vector<double> vectdparam30;
    std::vector<double> vectdparam31;
    std::vector<double> vectdparam32;
    std::vector<double> vectdparam33;
    std::vector<double> vectdparam34;
    std::vector<double> vectdparam35;
    std::vector<double> vectdparam36;
    std::vector<double> vectdparam37;
    std::vector<double> vectdparam38;
    std::vector<double> vectdparam39;
    std::vector<double> vectdparam40;
    std::vector<double> vectdparam41;
    std::vector<double> vectdparam42;
    std::vector<double> vectdparam43;
    std::vector<double> vectdparam44;
    std::vector<double> vectdparam45;
    std::vector<double> vectdparam46;
    std::vector<double> vectdparam47;
    std::vector<double> vectdparam48;
    std::vector<double> vectdparam49;
    std::vector<double> vectdparam50;
    std::vector<int> vectiparam1;
    std::vector<int> vectiparam2;
    std::vector<int> vectiparam3;
    std::vector<int> vectiparam4;
    std::vector<int> vectiparam5;
    std::vector<int> vectiparam6;
    std::vector<int> vectiparam7;
    std::vector<int> vectiparam8;
    std::vector<int> vectiparam9;
    std::vector<int> vectiparam10;
    std::multimap<int, double> mmapidparam1;
    std::multimap<int, double> mmapidparam2;
    std::multimap<int, double> mmapidparam3;
    std::multimap<int, double> mmapidparam4;
    std::multimap<int, double> mmapidparam5;
    std::multimap<int, double> mmapidparam6;
    std::multimap<int, double> mmapidparam7;
    std::multimap<int, double> mmapidparam8;
    std::multimap<string, double> mmapsdparam1;
    std::multimap<string, double> mmapsdparam2;
    std::multimap<string, double> mmapsdparam3;
    std::multimap<string, double> mmapsdparam4;
    std::multimap<string, double> mmapsdparam5;
    std::multimap<string, double> mmapsdparam6;
    std::multimap<string, double> mmapsdparam7;
    std::multimap<string, double> mmapsdparam8;
    std::multimap<string, types::NodeId> mmapsnparam1;
    std::multimap<string, types::NodeId> mmapsnparam2;
    std::multimap<string, types::NodeId> mmapsnparam3;
    std::multimap<string, types::NodeId> mmapsnparam4;
    std::multimap<types::NodeId, int> mmapniparam1;
    std::multimap<types::NodeId, int> mmapniparam2;
    std::multimap<types::NodeId, int> mmapniparam3;
    std::multimap<types::NodeId, int> mmapniparam4;
    std::multimap<types::NodeId, int> mmapniparam5;
    std::multimap<types::NodeId, int> mmapniparam6;
    std::multimap<types::NodeId, int> mmapniparam7;
    std::multimap<types::NodeId, int> mmapniparam8;
    std::multimap<types::NodeId, int> mmapniparam9;
    std::multimap<types::NodeId, int> mmapniparam10;
    std::multimap<types::NodeId, double> mmapndparam1;
    std::multimap<types::NodeId, double> mmapndparam2;
    std::vector<map<string, double> > vectmapsdparam1;
    std::vector<map<int, double> > vectmapsdparam2;
    std::vector<map<int, double> > vectmapsdparam3;
    std::map<int, int> mapiiparam1;
    std::map<int, int> mapiiparam2;
    std::map<int, double> mapidparam1;
    std::map<int, double> mapidparam2;
    std::map<int, double> mapidparam3;
    std::map<int, double> mapidparam4;
    std::map<int, string> mapisparam1;
    std::vector<map<int, int> > vectmapiiparam1;
    std::vector<map<int, double> > vectmapidparam1;
    std::vector<map<types::NodeId, double> > vectmapndparam1;
    std::vector<multimap<types::NodeId, int> > vectmmapniparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam2;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam3;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam4;
    std::vector<multimap<int, double> > vectmmapidparam1;
    std::vector<vector<double> > vovd1;
    std::vector<vector<double> > vovd2;
    std::vector<vector<double> > vovd3;
    std::vector<vector<double> > vovd4;
    std::vector<vector<double> > vovd5;
    std::vector<vector<double> > vovd6;
    std::vector<vector<double> > vovd7;
    std::vector<vector<double> > vovd8;
    std::vector<vector<double> > vovd9;
    std::vector<vector<double> > vovd10;
    std::vector<vector<types::NodeId> > vovn1;
    std::vector<vector<int> > vovi1;
    std::vector<vector<int> > vovi2;
    std::vector<vector<int> > vovi3;
    std::vector<vector<int> > vovi4;
    std::vector<vector<int> > vovi5;
    std::vector<vector<vector<double> > > vovovd1;
    std::vector<vector<vector<double> > > vovovd2;
    std::vector<vector<vector<double> > > vovovd3;
    int int1;
    int int2;
    int int3;
    int int4;
    int int5;
    double double1;
    double double2;
    double double3;
    double double4;
    double double5;
    double double6;
    double double7;
    double double8;
    double double9;
    double double10;
    double double11;
    double double12;
    double double13;
    double double14;
    double double15;
    double double16;
    double double17;
    double double18;
    double double19;
    double double20;
    double double21;
    double double22;
    double double23;
    double double24;
    double double25;
    double double26;
    double double27;
    double double28;
    double double29;
    double double30;
    multimap<int, double> mmapidparam_init_weight_per_szgroup;
    vector<vector<double> > vovd_species_interactions_mortality_proportion_matrix;
};

}

static
VesselsData loadLocalData(SimModel &model, int month, int quarter, int semester, std::string fname, std::string folder)
{
    auto quarterString = std::to_string(quarter);
    auto semesterString = std::to_string(semester);

    VesselsData loadedData;

    // read general vessel features
    // (quarter specific, mainly because of the gamma parameters)
    vector<string> vesselids;
    vector<int> vid_is_actives;
    vector<int> vid_is_part_of_ref_fleets;
    vector<double> speeds;
    vector<double> fuelcons;
    vector<double> lengths;
    vector<double> vKWs;
    vector<double> carrycapacities;
    vector<double> tankcapacities;
    vector<double> nbfpingspertrips;
    vector<double> resttime_par1s;
    vector<double> resttime_par2s;
    vector<double> av_trip_duration;
    vector<double> mult_fuelcons_when_steaming;
    vector<double> mult_fuelcons_when_fishing;
    vector<double> mult_fuelcons_when_returning;
    vector<double> mult_fuelcons_when_inactive;
    vector<int> firm_ids;
    vector<VesselCalendar> calendars;

    // paramsForLoad.sparam1; // a_month
    // quarterString;// a_quarter
    // semesterString; //a_semester
    // paramsForLoad.iparam1; //nbpops
    // paramsForLoad.iparam2; //NBAGE
    // paramsForLoad.iparam3; //NBSZGROUP

    int selected_vessels_only = 0;


    if (!read_vessels_features(quarterString, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                               speeds, fuelcons, lengths, vKWs,
                               carrycapacities, tankcapacities, nbfpingspertrips,
                               resttime_par1s, resttime_par2s, av_trip_duration,
                               mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                               mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids,
                               fname, folder, selected_vessels_only, calendars)) {
        std::cerr << "Cannot read vessel features.\n";
        throw std::runtime_error("Cannot read vessel features.");
    }


    vector<double> this_vessel_nb_crews;
    vector<double> annual_other_incomes;
    vector<double> landing_costs_percents;
    vector<double> crewshare_and_unpaid_labour_costs_percents;
    vector<double> other_variable_costs_per_unit_efforts;
    vector<double> annual_insurance_costs_per_crews;
    vector<double> standard_labour_hour_opportunity_costss;
    vector<double> standard_annual_full_time_employement_hourss;
    vector<double> other_annual_fixed_costss;
    vector<double> vessel_values;
    vector<double> annual_depreciation_rates;
    vector<double> opportunity_interest_rates;
    vector<double> annual_discount_rates;

    if (quarter == 1)
    { // annual data
        cout << "read_vessels_economic_features() in loadVesselsImpl()" << endl;
        if (!read_vessels_economics_features(
            vesselids,
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
            annual_discount_rates,
            fname, folder
        )) {
            std::cerr << "Cannot read vessel economic features.\n";
            throw std::runtime_error("Cannot read vessel economic features");
        }

    }

    cout << "read_vessels_economic_features() in loadVesselsImpl()..ok" << endl;

    // read the more complex objects (i.e. when several info for a same vessel)...
    // also quarter specific but semester specific for the betas because of the survey design they are comning from...
    auto fgrounds = read_fgrounds(quarterString, fname, folder);
    auto fgrounds_init = read_fgrounds_init(quarterString, fname, folder);
    auto harbours = read_harbours(quarterString, fname, folder);

    //cout << "read_freq_fgrounds() in loadVesselsImpl().." << endl;
    multimap<string, double> freq_fgrounds = read_freq_fgrounds(quarterString, fname,
                                                                folder);
    //cout << "read_freq_fgrounds() in loadVesselsImpl()..ok" << endl;
    //cout << "read_freq_fgrounds_init() in loadVesselsImpl().." << endl;
    multimap<string, double> freq_fgrounds_init = read_freq_fgrounds_init(quarterString,
                                                                          fname,
                                                                          folder);
    //cout << "read_freq_fgrounds_init() in loadVesselsImpl()..ok" << endl;
    //cout << "read_freq_harbours() in loadVesselsImpl().." << endl;
    multimap<string, double> freq_harbours = read_freq_harbours(quarterString, fname,
                                                                folder);
    //cout << "read_freq_harbours() in loadVesselsImpl()..ok" << endl;
    //cout << "read_vessels_betas() in loadVesselsImpl().." << endl;
    multimap<string, double> vessels_betas = read_vessels_betas(semesterString, fname,
                                                                folder);
    //cout << "read_vessels_betas() in loadVesselsImpl()..ok" << endl;
    //cout << "read_vessels_tacs() in loadVesselsImpl().." << endl;
    multimap<string, double> vessels_tacs = read_vessels_tacs(semesterString, fname,
                                                              folder);
    //cout << "read_vessels_tacs() in loadVesselsImpl()..ok" << endl;
   

    multimap<string, double> fishing_credits;
    if (model.scenario().dyn_alloc_sce.option(Options::fishing_credits)) {
        fishing_credits = read_initial_fishing_credits(fname, folder);
    }



    // loop over vids
    vector<multimap<types::NodeId, int> > vect_of_possible_metiers_mmap(vesselids.size());
    vector<multimap<types::NodeId, double> > vect_of_freq_possible_metiers_mmap(vesselids.size());
    vector<multimap<types::NodeId, double> > vect_of_gshape_cpue_per_stk_on_nodes_mmap(vesselids.size());
    vector<multimap<types::NodeId, double> > vect_of_gscale_cpue_per_stk_on_nodes_mmap(vesselids.size());

    //cout << "a data load loop over vessel " << endl;

    for (unsigned int i = 0; i < vesselids.size(); i++) {
        outc(cout << "a data load loop over vessel " << i << endl);

        // read vessel and quarter specific multimap
        // quarter specific to capture a piece of seasonality in the fishnig activity
        //cout << "read_possible_metiers for vessel " << i << endl;
        vect_of_possible_metiers_mmap.at(i) = read_possible_metiers(quarterString, vesselids[i],
                                                                    fname, folder);
        //cout << "read_possible_metiers for vessel " << i << "..ok" << endl;
        //cout << "read_freq_possible_metiers for vessel " << i << endl;
        vect_of_freq_possible_metiers_mmap.at(i) = read_freq_possible_metiers(quarterString, vesselids[i],
                                                                              fname,
                                                                              folder);
        //cout << "read_freq_possible_metiers for vessel " << i << "..ok" << endl;

        ////cpue_per_stk_on_nodes = read_cpue_per_stk_on_nodes(a_quarter, vesselids[i], fname);
        //cout << "read_gshape_cpue_per_stk_on_nodes for vessel " << i << endl;
        vect_of_gshape_cpue_per_stk_on_nodes_mmap.at(i) = read_gshape_cpue_per_stk_on_nodes(quarterString,
                                                                                            vesselids[i],
                                                                                            fname,
                                                                                            folder);
        //cout << "read_gshape_cpue_per_stk_on_nodes for vessel " << i << "...ok" << endl;
        //cout << "read_gscale_cpue_per_stk_on_nodes for vessel " << i << endl;
        vect_of_gscale_cpue_per_stk_on_nodes_mmap.at(i) = read_gscale_cpue_per_stk_on_nodes(quarterString,
                                                                                            vesselids[i],
                                                                                            fname,
                                                                                            folder);
        //cout << "read_gscale_cpue_per_stk_on_nodes for vessel " << i << "..ok" << endl;



        //auto max_idx_possible_metiers = std::max_element(vect_of_possible_metiers_mmap.at(i).begin(), vect_of_possible_metiers_mmap.at(i).end(),
        //    [](const pair<types::NodeId, int>& p1, const pair<types::NodeId, int>& p2) {
        //        return p1.second < p2.second; });
        //cout << "check here!" << endl;
        //cout <<"  max_idx_possible_metiers->second in loader is " << max_idx_possible_metiers->second << endl;



    }

    //cout << "a data load loop over vessel..ok " << endl;

    outc(cout << "export back the loaded vessel data to simulator.cpp " << endl);

    //  export
    loadedData.vectsparam1 = vesselids;
    loadedData.mmapsnparam1 = harbours;
    loadedData.mmapsnparam2 = fgrounds;
    loadedData.mmapsnparam3 = fgrounds_init;
    loadedData.mmapsdparam1 = freq_harbours;
    loadedData.mmapsdparam2 = freq_fgrounds;
    loadedData.mmapsdparam3 = freq_fgrounds_init;
    loadedData.mmapsdparam4 = vessels_betas;
    loadedData.mmapsdparam5 = vessels_tacs;
    loadedData.mmapsdparam6 = fishing_credits;
    loadedData.vectmmapniparam1 = vect_of_possible_metiers_mmap;
    loadedData.vectmmapndparam1 = vect_of_freq_possible_metiers_mmap;
    loadedData.vectmmapndparam2 = vect_of_gshape_cpue_per_stk_on_nodes_mmap;
    loadedData.vectmmapndparam3 = vect_of_gscale_cpue_per_stk_on_nodes_mmap;
    loadedData.vectiparam1 = vid_is_actives;
    loadedData.vectiparam2 = vid_is_part_of_ref_fleets;
    loadedData.vectdparam1 = speeds;
    loadedData.vectdparam2 = fuelcons;
    loadedData.vectdparam3 = lengths;
    loadedData.vectdparam4 = vKWs;
    loadedData.vectdparam5 = carrycapacities;
    loadedData.vectdparam6 = tankcapacities;
    loadedData.vectdparam7 = nbfpingspertrips;
    loadedData.vectdparam8 = resttime_par1s;
    loadedData.vectdparam9 = resttime_par2s;
    loadedData.vectdparam10 = av_trip_duration;
    loadedData.vectdparam11 = mult_fuelcons_when_steaming;
    loadedData.vectdparam12 = mult_fuelcons_when_fishing;
    loadedData.vectdparam13 = mult_fuelcons_when_returning;
    loadedData.vectdparam14 = mult_fuelcons_when_inactive;
    loadedData.vectiparam3 = firm_ids;
    loadedData.vectcalendar1 = calendars;
    loadedData.vectdparam16 = this_vessel_nb_crews;
    loadedData.vectdparam17 = annual_other_incomes;
    loadedData.vectdparam18 = landing_costs_percents;
    loadedData.vectdparam19 = crewshare_and_unpaid_labour_costs_percents;
    loadedData.vectdparam20 = other_variable_costs_per_unit_efforts;
    loadedData.vectdparam21 = annual_insurance_costs_per_crews;
    loadedData.vectdparam22 = standard_labour_hour_opportunity_costss;
    loadedData.vectdparam23 = standard_annual_full_time_employement_hourss;
    loadedData.vectdparam24 = other_annual_fixed_costss;
    loadedData.vectdparam25 = vessel_values;
    loadedData.vectdparam26 = annual_depreciation_rates;
    loadedData.vectdparam27 = opportunity_interest_rates;
    loadedData.vectdparam28 = annual_discount_rates;
    
    //cout << "a data load...ok " << endl;
    
    return loadedData;
}

static
void loadVessels(SimModel &model, std::string fname, std::string folder, int month, int quarter, int semester)
{
    auto quarterString = std::to_string(quarter);
    auto semesterString = std::to_string(semester);

    auto loadedDataVessels = loadLocalData(model, month, quarter, semester, fname, folder);

    vector<types::NodeId> spe_fgrounds;
    vector<types::NodeId> spe_fgrounds_init;
    vector<types::NodeId> spe_harbours;
    vector<double> spe_freq_fgrounds;
    vector<double> spe_freq_fgrounds_init;
    vector<double> spe_freq_harbours;
    vector<double> spe_vessel_betas_per_pop;
    vector<double> spe_percent_tac_per_pop;
    vector<double> spe_fishing_credits;

    // LOOP OVER VESSELIDS:
    int nbvessels = loadedDataVessels.vectsparam1.size();

    auto vessels = vector<Vessel *>(nbvessels);
    multimap<types::NodeId, int> possible_metiers;
    multimap<types::NodeId, double> freq_possible_metiers;
    multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
    multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;

    for (unsigned int i = 0; i < nbvessels; i++) {
        string vname = loadedDataVessels.vectsparam1.at(i);
        // read the even more complex objects (i.e. when several info for a same vessel and a same ground)...
        // for creating the vessel object, search into the multimaps
        possible_metiers = loadedDataVessels.vectmmapniparam1.at(i);
        freq_possible_metiers = loadedDataVessels.vectmmapndparam1.at(i);
        gshape_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam2.at(i);
        gscale_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam3.at(i);
        spe_fgrounds = find_entries(loadedDataVessels.mmapsnparam2, vname);
        spe_fgrounds_init = find_entries(loadedDataVessels.mmapsnparam3, vname);
        spe_freq_fgrounds = find_entries_s_d(loadedDataVessels.mmapsdparam2, vname);
        spe_freq_fgrounds_init = find_entries_s_d(loadedDataVessels.mmapsdparam3, vname);
        spe_harbours = find_entries(loadedDataVessels.mmapsnparam1, vname);
        spe_freq_harbours = find_entries_s_d(loadedDataVessels.mmapsdparam1, vname);
        spe_vessel_betas_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam4, vname);
        spe_percent_tac_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam5, vname);

        if (spe_vessel_betas_per_pop.size() != model.config().nbpops) {
            std::stringstream er;
            er << "Error while reading: vessel_betas_per_pop: check the dimension i.e. model.config().nbpops is" <<
               model.config().nbpops << " while spe_vessel_betas_per_pop.size() is " <<
               spe_vessel_betas_per_pop.size() << " for vessel " << vname;
            throw std::runtime_error(er.str());

            //possibly, fix dim in R for the oldest dataset:
            //ves <- do.call ("rbind.data.frame", lapply(split(ves, f=ves$VE_REF), function(x) x[1:model.config().nbpops,]))

        }

        // dyn sce.
        if (model.scenario().dyn_alloc_sce.option(Options::fishing_credits)) {
            tout(cout << "Read in fishing credits for this vessel " << loadedDataVessels.vectsparam1.at(i) << endl);
            spe_fishing_credits = find_entries_s_d(loadedDataVessels.mmapsdparam6, loadedDataVessels.vectsparam1.at(i));
            for (int icr = 0; icr < spe_fishing_credits.size(); ++icr) {
                spe_fishing_credits.at(icr) = spe_fishing_credits.at(icr) * model.scenario().total_amount_credited;
            }

            // complete to 3 values for tariff per node because we expect tariff all, tariff pop, and tariff benthos
            while (spe_fishing_credits.size() <= 3) { spe_fishing_credits.push_back(0); }
            cout << "Fishing credits 0 for this vessel " << loadedDataVessels.vectsparam1.at(i) << " is "
                 << spe_fishing_credits.at(0) << endl;

        }

        if (model.scenario().dyn_alloc_sce.option(Options::reduced_speed_20percent)) {
            // a decrease of vessel speed by 20%...
            loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i) * 0.8;
            // corresponds to a decrease by 48.8% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i) * 0.512;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }

        if (model.scenario().dyn_alloc_sce.option(Options::gear_fuel_efficiency_gain_30percent)) {
              // decrease consumption when fishing by altering mult_fuelcons_when_fishing
            loadedDataVessels.vectdparam12.at(i) = loadedDataVessels.vectdparam12.at(i) * 0.70;
        }

        if (model.scenario().dyn_alloc_sce.option(Options::reduced_speed_30percent)) {
            // a decrease by 30%...
            loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i) * 0.7;
            // corresponds to a decrease by 65.7% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i) * 0.343;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }

        if (model.scenario().dyn_alloc_sce.option(Options::reduced_speed_10percent)) {
            // a decrease by 10%...
            loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i) * 0.9;
            // corresponds to a decrease by 30% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i) * 0.7;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }



        // choose a departure (node) harbour for this vessel according to the observed frequency in data
        types::NodeId start_harbour;
        if (!spe_harbours.empty()) {
            // need to convert in array, see myRutils.cpp
            auto one_harbour = do_sample(1, spe_harbours.size(), spe_harbours, spe_freq_harbours);
            start_harbour = one_harbour[0];
        } else {
            // if missing info for a given vessel for this quarter
            outc(cout << "no specified harbour in this quarter for this vessel..." << endl);
            // CAUTION: LIKE A MAGIC NUMBER HERE!!!
            start_harbour = find_entries(loadedDataVessels.mmapsnparam1, loadedDataVessels.vectsparam1.at(0))[0];
            spe_harbours.push_back(start_harbour);
            spe_freq_harbours.push_back(1);
            outc(cout << "then take node: " << start_harbour << endl);
        }

        cout << "Creating vessel " << i << endl;

        vessels[i] = new Vessel(model.nodes().at(start_harbour.toIndex()),
                                i,
                                loadedDataVessels.vectsparam1.at(i),
                                model.config().nbpops,
                                model.config().nbmets,
                                model.config().nbszgroups,
                                spe_harbours,
                                spe_fgrounds,
                                spe_fgrounds_init,
                                spe_freq_harbours,
                                spe_freq_fgrounds,
                                spe_freq_fgrounds_init,
                                spe_vessel_betas_per_pop,
                                spe_percent_tac_per_pop,
                                possible_metiers,
                                freq_possible_metiers,
                                gshape_cpue_per_stk_on_nodes,
                                gscale_cpue_per_stk_on_nodes,
                                loadedDataVessels.vectiparam1.at(i),
                                loadedDataVessels.vectiparam2.at(i),
                                loadedDataVessels.vectdparam1.at(i),
                                loadedDataVessels.vectdparam2.at(i),
                                loadedDataVessels.vectdparam3.at(i),
                                loadedDataVessels.vectdparam4.at(i),
                                loadedDataVessels.vectdparam5.at(i),
                                loadedDataVessels.vectdparam6.at(i),
                                loadedDataVessels.vectdparam7.at(i),
                                loadedDataVessels.vectdparam8.at(i),
                                loadedDataVessels.vectdparam9.at(i),
                                loadedDataVessels.vectdparam10.at(i),
                                loadedDataVessels.vectdparam11.at(i),
                                loadedDataVessels.vectdparam12.at(i),
                                loadedDataVessels.vectdparam13.at(i),
                                loadedDataVessels.vectdparam14.at(i),
                                loadedDataVessels.vectiparam3.at(i),
                                loadedDataVessels.vectcalendar1.at(i),
                                i < loadedDataVessels.vectdparam16.size() ? loadedDataVessels.vectdparam16.at(i) : 0,
                                i < loadedDataVessels.vectdparam17.size() ? loadedDataVessels.vectdparam17.at(i) : 0,
                                i < loadedDataVessels.vectdparam18.size() ? loadedDataVessels.vectdparam18.at(i) : 0,
                                i < loadedDataVessels.vectdparam19.size() ? loadedDataVessels.vectdparam19.at(i) : 0,
                                i < loadedDataVessels.vectdparam20.size() ? loadedDataVessels.vectdparam20.at(i) : 0,
                                i < loadedDataVessels.vectdparam21.size() ? loadedDataVessels.vectdparam21.at(i) : 0,
                                i < loadedDataVessels.vectdparam22.size() ? loadedDataVessels.vectdparam22.at(i) : 0,
                                i < loadedDataVessels.vectdparam23.size() ? loadedDataVessels.vectdparam23.at(i) : 0,
                                i < loadedDataVessels.vectdparam24.size() ? loadedDataVessels.vectdparam24.at(i) : 0,
                                i < loadedDataVessels.vectdparam25.size() ? loadedDataVessels.vectdparam25.at(i) : 0,
                                i < loadedDataVessels.vectdparam26.size() ? loadedDataVessels.vectdparam26.at(i) : 0,
                                i < loadedDataVessels.vectdparam27.size() ? loadedDataVessels.vectdparam27.at(i) : 0,
                                i < loadedDataVessels.vectdparam28.size() ? loadedDataVessels.vectdparam28.at(i) : 0
        );


        cout << "Creating vessel " << i << "..ok" << endl;

#if 0
        if(vessels[i]->get_other_variable_costs_per_unit_effort()==0 ) {
           cout << "debug here" << endl;
           cout << "i is " << i << endl;
           cout << "other_variable_costs_per_unit_efforts[i] is " << other_variable_costs_per_unit_efforts[i];
           cout << "other_variable_costs_per_unit_efforts.size() is " << other_variable_costs_per_unit_efforts.size();

            int aa; cin >> aa;
        }

#endif

        // Give super power to each vessel (so that he can consult the common tariff map for example)
        vessels[i]->set_map_of_nodes(model.nodes());

        if (model.scenario().dyn_alloc_sce.option(Options::fishing_credits)) {
            vessels[i]->set_fishing_credits(spe_fishing_credits);
        }


        // initialise the individual quota from global_TAC*percent_in_simu*percent_this_vessel
        if (model.is_tacs()) {
            for (unsigned int sp = 0; sp < model.populations().size(); sp++) {
                vessels.at(i)->set_individual_tac_this_pop(export_individual_tacs, 0, model.populations(),
                                                           model.config().implicit_pops, sp, 1,
                                                           0.0);
            }
        }

    
        
    

        // check
        outc(cout << "create vessel " << vessels[i]->get_idx() << " " << vessels[i]->get_name() << " "
                  << vessels[i]->get_nationality() << " on "
                  << vessels[i]->get_loc()->get_idx_node() << " with coordinates "
                  << vessels[i]->get_loc()->get_x() << " " << vessels[i]->get_loc()->get_y() << endl);

    }

    model.setVessels(vessels);
}

static
void reloadVessels(SimModel &model, std::string fname, std::string folder, int month, int quarter, int semester)
{
    auto quarterString = std::to_string(quarter);
    auto semesterString = std::to_string(semester);

    cout << "reloadVessels()...." << endl;
    auto loadedDataVessels = loadLocalData(model, month, quarter, semester, fname, folder);

    // LOOP OVER VESSELS
    int v = 0;
    for (auto vessel : model.vessels()) {
        dout(cout << "re-read data for vessel " << vessel->get_name() << endl);

        if (model.scenario().dyn_alloc_sce.option(Options::ExitVessels10Per)) {
            double exit_vessels_per_year = 0.1; //  this is a rate of vessel leaving per year
            if (model.calendar().isFirstDayOfYear(model.timestep())) {

                if ((rand() % 2) < exit_vessels_per_year) { vessel->set_vessel_exited(1); }

            }
        }

        if (model.quarter() == 1) {
            double new_vessel_value =
                    vessel->get_vessel_value() * (100 - vessel->get_annual_depreciation_rate()) /
                    100;
            vessel->set_vessel_value(new_vessel_value); // capital depreciation

            for (unsigned int pop = 0; pop < model.config().nbpops; ++pop)
                vessel->set_is_choked(pop, 0); // reinit at year start
        }

        auto possible_metiers = loadedDataVessels.vectmmapniparam1.at(v);
        auto freq_possible_metiers = loadedDataVessels.vectmmapndparam1.at(v);
        auto gshape_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam2.at(v);
        auto gscale_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam3.at(v);
        vector<string> vesselids = loadedDataVessels.vectsparam1;
        auto spe_fgrounds = find_entries(loadedDataVessels.mmapsnparam2, vesselids.at(v));
        auto spe_fgrounds_init = find_entries(loadedDataVessels.mmapsnparam3, vesselids.at(v));
        auto spe_harbours = find_entries(loadedDataVessels.mmapsnparam1, vesselids.at(v));
        auto spe_freq_fgrounds = find_entries_s_d(loadedDataVessels.mmapsdparam2, vesselids.at(v));
        auto spe_freq_fgrounds_init = find_entries_s_d(loadedDataVessels.mmapsdparam3, vesselids.at(v));
        auto spe_freq_harbours = find_entries_s_d(loadedDataVessels.mmapsdparam1, vesselids.at(v));
        auto spe_vessel_betas_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam4, vesselids.at(v));

        vector<double> spe_percent_tac_per_pop;
        if (model.is_tacs()) {
            spe_percent_tac_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam5, vesselids.at(v));
        }
       
        if (model.scenario().dyn_alloc_sce.option(Options::fishing_credits))
        {
            if (model.calendar().isFirstDayOfYear(model.timestep())) {
                cout << "Re-read fishing credits for this vessel " << loadedDataVessels.vectsparam1.at(v) << endl;
                auto spe_fishing_credits = find_entries_s_d(loadedDataVessels.mmapsdparam6, loadedDataVessels.vectsparam1.at(v));
                for (int icr = 0; icr < spe_fishing_credits.size(); ++icr)
                {
                    spe_fishing_credits.at(icr) = spe_fishing_credits.at(icr) * model.scenario().total_amount_credited;


                }
                // complete to 3 values for tariff per node because we expect tariff all, tariff pop, and tariff benthos
                while (spe_fishing_credits.size() <= 3) { spe_fishing_credits.push_back(0); }
                cout << "Fishing credits 0 for this vessel " << loadedDataVessels.vectsparam1.at(v) << " is "
                    << spe_fishing_credits.at(0) << endl;

                vessel->set_fishing_credits(spe_fishing_credits);
            }
        }


        // correct if missing harbour for this quarter
        if (spe_harbours.empty()) {
            // if missing info for a given vessel for this quarter
            outc(cout << "no specified harbour in this quarter for this vessel..." << endl);
            // CAUTION: TAKEN FROM THE PREVIOUS QUARTER!
            auto start_harbour = vessel->get_harbours()[0];
            spe_harbours.push_back(start_harbour);
            spe_freq_harbours.push_back(1);
            outc(cout << "then take node: " << start_harbour << endl);
        }

        // RE-SET VESSELS..
        dout(cout << "re-set vessels step1..." << endl);
        vessel->set_resttime_par1(loadedDataVessels.vectdparam8.at(v));
        vessel->set_resttime_par2(loadedDataVessels.vectdparam9.at(v));
        vessel->set_av_trip_duration(loadedDataVessels.vectdparam10.at(v));

        if (model.scenario().dyn_alloc_sce.option(Options::area_closure)) {
            if (!read_metier_quarterly_closures(model.nodes(), quarterString,
                                                model.scenario().a_graph_name,
                                                fname,
                                                folder)) {
                std::cerr << "this went wrong reading metier_quarterly_closure.dat files, quitting\n\n";
                throw std::runtime_error("this went wrong reading metier_quarterly_closure.dat files, quitting");
            }
        }


        vessel->set_spe_fgrounds(spe_fgrounds);
        vessel->set_spe_fgrounds_init(spe_fgrounds_init);
        vessel->set_spe_harbours(spe_harbours);
        vessel->set_spe_freq_fgrounds(spe_freq_fgrounds);
        vessel->set_spe_freq_fgrounds_init(spe_freq_fgrounds_init);
        vessel->set_spe_freq_harbours(spe_freq_harbours);
        vector<double> init_for_fgrounds(vessel->get_fgrounds().size());
        for (unsigned int i = 0; i < init_for_fgrounds.size(); i++) {
            init_for_fgrounds[i] = 0;
        }
        vessel->set_spe_cumcatch_fgrounds(init_for_fgrounds);
        vessel->set_spe_cumdiscard_fgrounds(init_for_fgrounds);
        vessel->set_spe_experienced_bycatch_prop_on_fgrounds(init_for_fgrounds);
        vessel->set_spe_experienced_avoided_stks_bycatch_prop_on_fgrounds(init_for_fgrounds);
        vessel->set_spe_cumeffort_per_trip_per_fgrounds(init_for_fgrounds);
        vessel->set_spe_experiencedcpue_fgrounds(init_for_fgrounds);
        vessel->set_spe_betas_per_pop(spe_vessel_betas_per_pop);

        // uncomment for use if TACs are semester-based....otherwise don´t refresh the annual TAC!
        //if (semester) vessel->set_spe_percent_tac_per_pop(spe_percent_tac_per_pop);

        vessel->set_spe_possible_metiers(possible_metiers);
        vessel->set_spe_freq_possible_metiers(freq_possible_metiers);
        vessel->updateCalendar(loadedDataVessels.vectcalendar1.at(v));


        // ...also for the particular cpue_nodes_species element
        dout(cout << "re-set vessels step2..." << endl);
        vector<types::NodeId> gshape_name_nodes_with_cpue;
        for (auto iter = gshape_cpue_per_stk_on_nodes.begin(); iter != gshape_cpue_per_stk_on_nodes.end();
             iter = gshape_cpue_per_stk_on_nodes.upper_bound(iter->first)) {
            gshape_name_nodes_with_cpue.push_back(iter->first);
        }
        // sort and unique
        sort(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
        auto it = std::unique(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
        gshape_name_nodes_with_cpue.resize(std::distance(gshape_name_nodes_with_cpue.begin(), it));

        // init cpue_nodes_species for this vessel
        int nbnodes = gshape_name_nodes_with_cpue.size();
        // init the vector of vector with Os
        vessel->init_gshape_cpue_nodes_species(nbnodes, model.config().nbpops);
        // init the vector of vector with Os
        vessel->init_gscale_cpue_nodes_species(nbnodes, model.config().nbpops);
        for (unsigned int n = 0; n < gshape_name_nodes_with_cpue.size(); n++) {
            // look into the multimap...
            auto gshape_cpue_species = find_entries(gshape_cpue_per_stk_on_nodes,
                                                    gshape_name_nodes_with_cpue[n]);
            // look into the multimap...
            auto gscale_cpue_species = find_entries(gscale_cpue_per_stk_on_nodes,
                                                    gshape_name_nodes_with_cpue[n]);
            if (!gshape_cpue_species.empty()) {
                // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                vessel->set_gshape_cpue_nodes_species(n, gshape_cpue_species);
                // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                vessel->set_gscale_cpue_nodes_species(n, gscale_cpue_species);
            }
        }

        // need to compute expected cpue (averaged over node but cumulated over species)
        // for this particular vessel, in order to scale the prior guess (see below)
        dout(cout << "re-set vessels step3..." << endl);
        double expected_cpue = 0;
        vector<vector<double> > gshape_cpue_nodes_species = vessel->get_gshape_cpue_nodes_species();
        vector<vector<double> > gscale_cpue_nodes_species = vessel->get_gscale_cpue_nodes_species();
        const auto &fgrounds = vessel->get_fgrounds();
        vector<double> expected_cpue_this_pop(model.config().nbpops);
        for (int pop = 0; pop < model.config().nbpops; pop++) {
            vector<double> cpue_per_fground(fgrounds.size());
            // init
            expected_cpue_this_pop.at(pop) = 0;

            // compute cpue on nodes
            for (unsigned int g = 0; g < fgrounds.size(); g++) {
                // look into the vector of vector....
                double a_shape = gshape_cpue_nodes_species.at(g).at(pop);
                // look into the vector of vector....
                double a_scale = gscale_cpue_nodes_species.at(g).at(pop);


                // a dangerous fix:
                if (a_shape < 0 || a_scale < 0) {

                    cout << "Something weird with the Gamma parameters: some negative values loaded...."
                         << endl;
                    //for(size_t f = 0; f < fgrounds.size(); ++f)
                    //{
                    //cout <<  " this gr  gscale is: " << gscale_cpue_nodes_species.at(f).at(pop) << endl;
                    //cout <<  " this gr  of gshape is: " << gshape_cpue_nodes_species.at(f).at(pop) << endl;
                    //}
                    a_shape = 1;
                    a_scale = 0;
                }

                cpue_per_fground.at(g) = rgamma(a_shape, a_scale);
                dout(cout << "cpue_per_fground.at(g)" << cpue_per_fground.at(g) << endl);
                //if(vessel->get_name()=="DNK000041435") cout  << "cpue_per_fground.at(g)" <<cpue_per_fground.at(g) << endl;
            }

            dout(cout << "re-set vessels step3.1..." << endl);

            // compute the average cpue for this pop across all nodes
            for (unsigned int g = 0; g < fgrounds.size(); g++) {
                expected_cpue_this_pop.at(pop) += cpue_per_fground.at(g);
            }
            // do the mean
            expected_cpue_this_pop.at(pop) = expected_cpue_this_pop.at(pop) / fgrounds.size();

            // sum over pop
            expected_cpue += expected_cpue_this_pop.at(pop);
        }

        dout(cout << "expected_cpue for this vessel is " << expected_cpue << endl);

        // init at 0 cumcatch and cumeffort per trip,
        // init at best guest the experiencedcpue_fgrounds
        dout(cout << "re-set vessels step4..." << endl);
        dout(cout << "init dynamic object related to fgrounds" << endl);
        const vector<double> &a_freq_fgrounds = vessel->get_freq_fgrounds();
        vector<double> a_init_for_fgrounds(fgrounds.size());
        vector<double> a_cumeffort_per_trip_per_fgrounds = a_init_for_fgrounds;
        vector<double> a_cumeffort_per_yearquarter_per_fgrounds = a_init_for_fgrounds;
        vector<double> a_cumcatch_fgrounds = a_init_for_fgrounds;
        vector<vector<double> > a_cumeffort_per_trip_per_fgrounds_per_met(fgrounds.size(), vector<double>(model.config().nbmets));
        vector<double> a_experienced_bycatch_prop_on_fgrounds = a_init_for_fgrounds;
        vector<double> a_experienced_avoided_stks_bycatch_prop_on_fgrounds = a_init_for_fgrounds;

        // overall (new objects each quarter)
        vector<double> a_cumdiscard_fgrounds = a_init_for_fgrounds;
        vector<double> a_experiencedcpue_fgrounds = a_init_for_fgrounds;
        vector<double> a_freq_experiencedcpue_fgrounds = a_init_for_fgrounds;

        // or per pop (new objects each quarter)
        vector<vector<double> > a_cumcatch_fgrounds_per_pop(fgrounds.size(), vector<double>(model.config().nbpops));
        Vessel::CumcatchFgroundsPerMetPerPop a_cumcatch_fgrounds_per_met_per_pop(fgrounds.size(),
                                                                                 model.config().nbmets,
                                                                                 model.config().nbpops);
        vector<vector<double> > a_cumdiscard_fgrounds_per_pop(fgrounds.size(), vector<double>(model.config().nbpops));
        vector<vector<double> > a_experiencedcpue_fgrounds_per_pop(fgrounds.size(),
                                                                   vector<double>(model.config().nbpops));

        // or per met per pop (new objects each quarter)
        vector<vector<vector<double> > > a_experiencedcpue_fgrounds_per_met_per_pop(fgrounds.size(),
                                                                                    vector<vector<double>>(
                                                                                            model.config().nbmets,
                                                                                            vector<double>(
                                                                                                    model.config().nbpops)));
        vector<vector<double> > a_freq_experiencedcpue_fgrounds_per_pop(fgrounds.size(),
                                                                        vector<double>(model.config().nbpops));
        vector<vector<vector<double> > > a_freq_experiencedcpue_fgrounds_per_met_per_pop(fgrounds.size(),
                                                                                         vector<vector<double>>(
                                                                                                 model.config().nbmets,
                                                                                                 vector<double>(
                                                                                                         model.config().nbpops)));

        // or per yearquarter per pop (CARRYING OVER QUARTERS - for Option experiencedCPUEsPerYearQuarter: THIS CANNOT WORK IF THE VESSEL HAS NOT STRICTLY THE SAME GROUND FOR EACH QUARTER)
        //initialisation of objects useful for if (model.scenario().dyn_alloc_sce.option(Options::experiencedCPUEsPerYearQuarter))
        int nbyearquarters = 11 * 4;
        vector<vector<vector<double> > > a_cumcatch_fgrounds_per_yearquarter_per_pop = vessel->get_cumcatch_fgrounds_per_yearquarter_per_pop();
        vector<vector<vector<double> > > a_experiencedcpue_fgrounds_per_yearquarter_per_pop = vessel->get_experiencedcpue_fgrounds_per_yearquarter_per_pop();
        vector<vector<vector<double> > > a_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop = vessel->get_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop();
        int q = model.quarter() - 1;
        int y = model.year() - 1;


        for (unsigned int g = 0; g < fgrounds.size(); g++) {
            a_cumcatch_fgrounds[g] = 0;
            a_cumdiscard_fgrounds[g] = 0;
            a_experienced_bycatch_prop_on_fgrounds[g] = 0;
            a_experienced_avoided_stks_bycatch_prop_on_fgrounds[g] = 0;
            a_cumeffort_per_trip_per_fgrounds[g] = 0;
            a_cumeffort_per_yearquarter_per_fgrounds[g] = 0;
            a_experiencedcpue_fgrounds[g] = a_freq_fgrounds[g] * expected_cpue;
            // this should be init so that it constitutes a good qualified guess to be a prior in the bayesian formula...
            // first condition: init different to 0 to allow the ground to be chosen even if it has not been visited yet...
            // second condition: to avoid starting from 0 cpue, init accounting for prior from frequency of visit from the data
            // third condition: to scale the start cpue, multiply by the expectancy of the cpue for this particular vessel
            dout(cout << "a_experiencedcpue_fgrounds" << a_experiencedcpue_fgrounds[g] << endl);
            // init the ones per pop
            for (int pop = 0; pop < model.config().nbpops; pop++)
            {
                // init
                a_cumcatch_fgrounds_per_pop[g][pop] = 0;
                //a_cumdiscard_fgrounds_per_pop[g][pop] = 0;
                a_experiencedcpue_fgrounds_per_pop[g][pop] =
                        a_freq_fgrounds[g] * expected_cpue_this_pop.at(pop);
                if (model.quarter() == 1 && model.year() == 1)
                {
                    for (int met = 0; met < model.config().nbmets; met++)
                    {
                        a_cumeffort_per_trip_per_fgrounds_per_met[g][met] = 0;
                        a_cumcatch_fgrounds_per_met_per_pop.zero(g, met, pop);
                        a_experiencedcpue_fgrounds_per_met_per_pop[g][met][pop] =
                                a_freq_fgrounds[g] * expected_cpue_this_pop.at(pop); // init is not metier-specific
                    }
                }
                if (model.scenario().dyn_alloc_sce.option(Options::experiencedCPUEsPerYearQuarter))
                {
                    if (a_cumcatch_fgrounds_per_yearquarter_per_pop.size() != fgrounds.size())
                    {
                        cout << "experiencedCPUEsPerYearQuarter cannot carry info over quarters: correct input files: the Option requires strictly identical vesselsspe_fgrounds_quarterXX.dat files" << endl;
                    }
                    if (model.year() == 1)
                    {
                        a_cumcatch_fgrounds_per_yearquarter_per_pop[g][q][pop] = 0;
                        a_experiencedcpue_fgrounds_per_yearquarter_per_pop[g][q][pop] =
                            a_freq_fgrounds[g] * expected_cpue_this_pop.at(pop); // init 
                    }
                }
            }
        }
        // per total...
        dout(cout << "re-set vessels step5..." << endl);
        vessel->set_cumcatch_fgrounds(a_cumcatch_fgrounds);
        vessel->set_cumdiscard_fgrounds(a_cumdiscard_fgrounds);
        vessel->set_experienced_bycatch_prop_on_fgrounds(a_experienced_bycatch_prop_on_fgrounds);
        vessel->set_experienced_avoided_stks_bycatch_prop_on_fgrounds(
                a_experienced_avoided_stks_bycatch_prop_on_fgrounds);
        
        // in overall
        vessel->set_cumeffort_per_trip_per_fgrounds(a_cumeffort_per_trip_per_fgrounds);
        vessel->set_cumeffort_per_yearquarter_per_fgrounds(a_cumeffort_per_yearquarter_per_fgrounds);
        vessel->set_experiencedcpue_fgrounds(a_experiencedcpue_fgrounds);
        vessel->set_freq_experiencedcpue_fgrounds(a_freq_experiencedcpue_fgrounds);
        // compute for the first time, to get freq_experiencedcpue_fgrounds...
        vessel->compute_experiencedcpue_fgrounds();
     
        // ...or per pop
        vessel->set_cumcatch_fgrounds_per_pop(a_cumcatch_fgrounds_per_pop);
        vessel->set_cumdiscard_fgrounds_per_pop(a_cumdiscard_fgrounds_per_pop);
        vessel->set_experiencedcpue_fgrounds_per_pop(a_experiencedcpue_fgrounds_per_pop);
        vessel->set_freq_experiencedcpue_fgrounds_per_pop(a_freq_experiencedcpue_fgrounds_per_pop);
        // compute for the first time, to get freq_experiencedcpue_fgrounds_per_pop...
        vessel->compute_experiencedcpue_fgrounds_per_pop();

        // ...or per met per pop
        vessel->set_cumeffort_per_trip_per_fgrounds_per_met(a_cumeffort_per_trip_per_fgrounds_per_met);
        vessel->set_cumcatch_fgrounds_per_met_per_pop(a_cumcatch_fgrounds_per_met_per_pop);
        vessel->set_experiencedcpue_fgrounds_per_met_per_pop(a_experiencedcpue_fgrounds_per_met_per_pop);
        vessel->set_freq_experiencedcpue_fgrounds_per_met_per_pop(a_freq_experiencedcpue_fgrounds_per_met_per_pop);
        vessel->compute_experiencedcpue_fgrounds_per_met_per_pop();

        // ...or per yearquarter per pop (caution: reload only the first y)
        if (model.scenario().dyn_alloc_sce.option(Options::experiencedCPUEsPerYearQuarter))
        {
            if (model.year() == 1) vessel->set_cumcatch_fgrounds_per_yearquarter_per_pop(a_cumcatch_fgrounds_per_yearquarter_per_pop);
            if (model.year() == 1) vessel->set_experiencedcpue_fgrounds_per_yearquarter_per_pop(a_experiencedcpue_fgrounds_per_yearquarter_per_pop);
            if (model.year() == 1) vessel->set_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop(a_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop);
            if (model.year() == 1) vessel->compute_experiencedcpue_fgrounds_per_yearquarter_per_pop(y, q);
        }

        // note that, at the start of the simu, freq of visit will be equivalent to a_freq_fgrounds
        // and then freq of visit will be updated (via the bayes rule) trip after trip from this initial freqency...
        // the expected_cpue is to scale to the encountered cpue i.e. freq of visit will decrease if experienced cpue < expected cpue
        // and vice versa...

        //
        // to force re-computation of the fuel saving scenario

        // TODO check: nodeId=0 is a valid value. Perhaps it should be changed to InvalidNodeId ?
        vessel->set_previous_harbour_idx(types::NodeId(0));

        // send a message to the vessel to force it for a change in fishing grounds (for the vessels that are fishing now or on their way to fish)
        // because we have just changed the list of fishing grounds! so maybe some vessels are fishing on some nodes
        // that are no longer in the list...
        // message 1 means: "please, change of grounds as soon as possible"
        vessel->receive_message(1);

        cout << "re-read data for this vessel..." << vessel->get_name() << "...OK" << endl;
        ++v;
    }                     // end a_vesselid


}

void TextfileModelLoader::loadVessels(int year, int month, int quarter, int semester)
{
    if (year==1 && month == 1) {
        ::loadVessels(model(), p->folder_name_parameterization, p->inputfolder, month, quarter, semester);
    } else {
        ::reloadVessels(model(), p->folder_name_parameterization, p->inputfolder, month, quarter, semester);
    }
}
