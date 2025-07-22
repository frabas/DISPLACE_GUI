//
// Created by happycactus on 2/22/20.
//

#include "RuntimeTests.h"
#include "myutils.h"
#include "myRutils.h"
#include "helpers.h"
#include "SimModel.h"
#include "comstructs.h"

#include <iostream>

using namespace std;

namespace test {

void config(std::string const &a_graph_name, SimModel const &simModel)
{
    // check if config reading OK
    outc(cout << simModel.config().nbpops << "\n");
    for (unsigned int a_pop = 0; a_pop < simModel.config().implicit_pops.size(); a_pop++) {
        outc(cout << " " << simModel.config().implicit_pops.at(a_pop));
    }
    outc(cout << "\n");
    for (int a_pop = 0; a_pop < simModel.config().nbpops; a_pop++) {
        outc(cout << " " << simModel.config().calib_oth_landings.at(a_pop));
    }
    outc(cout << "\n");
    for (int a_pop = 0; a_pop < simModel.config().nbpops; a_pop++) {
        outc(cout << " " << simModel.config().calib_weight_at_szgroup.at(a_pop));
    }
    outc(cout << "\n");
    for (int a_pop = 0; a_pop < simModel.config().nbpops; a_pop++) {
        outc(cout << " " << simModel.config().calib_cpue_multiplier.at(a_pop));
    }
    outc(cout << "\n");

    auto const &scenario = simModel.scenario();

    outc(cout << scenario.dyn_alloc_sce.toString() << "\n");
    outc(cout << scenario.dyn_pop_sce.toString() << "\n");
    outc(cout << "biolsce " << scenario.biolsce << "\n");
    outc(cout << "fleetsce " << scenario.fleetsce << "\n");
    outc(cout << "freq_do_growth " << scenario.freq_do_growth << "\n");
    outc(cout << "freq_redispatch_the_pop " << scenario.freq_redispatch_the_pop << "\n");
    outc(cout << "a_graph " << scenario.a_graph << "\n");
    outc(cout << "a_graph_name " << a_graph_name << "\n");
    outc(cout << "nrow_coord " << scenario.nrow_coord << "\n");
    outc(cout << "nrow_graph " << scenario.nrow_graph << "\n");
    outc(cout << "a_port " << scenario.a_port << "\n");
    outc(cout << "graph res in km xy " << scenario.graph_res.at(0) << " " << scenario.graph_res.at(1) << "\n");
    outc(cout << "is_individual_vessel_quotas " << simModel.scenario().is_individual_vessel_quotas << "\n");
    outc(cout << "check_all_stocks_before_going_fishing " << scenario.check_all_stocks_before_going_fishing << "\n");

    if (scenario.dyn_alloc_sce.option(Options::fishing_credits)) {
        outc(cout << "tariff_pop.at(0) " << scenario.tariff_pop.at(0) << "\n");
        outc(cout << "freq_update_tariff_code " << scenario.freq_update_tariff_code << "\n");
        outc(cout << "update_tariffs_based_on_lpue_or_dpue_code " << scenario.update_tariffs_based_on_lpue_or_dpue_code
                  << "\n");
        outc(cout << "arbitary_breaks_for_tariff.at(0) " << scenario.arbitary_breaks_for_tariff.at(0) << "\n");
        outc(cout << "total_amount_credited " << scenario.total_amount_credited << "\n");
        outc(cout << "tariff_annual_hcr_percent_change " << scenario.tariff_annual_hcr_percent_change << "\n");
    }
}

void myUtils()
{
    dout(cout << "---------------------------" << "\n");
    dout(cout << "---------------------------" << "\n");
    dout(cout << " TEST MY UTILS             " << "\n");
    dout(cout << "---------------------------" << "\n");
    dout(cout << "---------------------------" << "\n");

    double MLS = 40;
    double L75 = 46.4;
    double L50 = 27.4;
    double S1 = L50 * log(3) / (L75 - L50);
    double S2 = S1 / L50;
    double result1 = simpson<myintegrand>(20, MLS, 100, S1, S2); // discards
    double result2 = simpson<myintegrand>(MLS, 70, 100, S1, S2); // landings
    dout(cout << result1 << "  " << result2 << "\n");


    vector<double> sel;
    sel.push_back(0.4015599);
    sel.push_back(0.4155306);
    sel.push_back(0.4296385);
    sel.push_back(0.4438615);
    sel.push_back(0.4581772);
    sel.push_back(0.4725623);
    sel.push_back(0.4869931);
    sel.push_back(0.5014455);
    sel.push_back(0.5158956);
    sel.push_back(0.5303191);
    sel.push_back(0.5446922);
    sel.push_back(0.5589912);
    sel.push_back(0.5731930);
    sel.push_back(0.5872752);
    sel.push_back(0.6012159);
    sel.push_back(0.6149945);
    sel.push_back(0.6285911);
    sel.push_back(0.6419871);
    sel.push_back(0.6551650);
    sel.push_back(0.6681085);
    sel.push_back(0.6808029);
    sel.push_back(0.6932348);
    sel.push_back(0.7053919);
    sel.push_back(0.7172638);
    sel.push_back(0.7288412);
    sel.push_back(0.7401162);
    sel.push_back(0.7510826);
    sel.push_back(0.7617352);
    sel.push_back(0.7720702);
    sel.push_back(0.7820852);
    sel.push_back(0.7917788);
    sel.push_back(0.8011509);
    sel.push_back(0.8102022);
    sel.push_back(0.8189347);
    sel.push_back(0.8273510);
    sel.push_back(0.8354546);
    sel.push_back(0.8432500);
    sel.push_back(0.8507421);
    sel.push_back(0.8579364);
    sel.push_back(0.8648390);
    sel.push_back(0.8714565);
    sel.push_back(0.8777957);
    sel.push_back(0.8838640);
    sel.push_back(0.8896689);
    sel.push_back(0.8952180);
    sel.push_back(0.9005192);
    sel.push_back(0.9055805);
    sel.push_back(0.9104099);
    sel.push_back(0.9150155);
    sel.push_back(0.9194053);
    sel.push_back(0.9235872);
    double result3 = trapezoidal(20 - 20, MLS - 20, sel); // landings
    double result4 = trapezoidal(MLS - 20, 70 - 20, sel); // landings
    cout << result3 << "  " << result4 << "\n";
}

void myRUtils()
{
    dout(cout << "---------------------------" << "\n");
    dout(cout << "---------------------------" << "\n");
    dout(cout << " TEST MY R UTILS           " << "\n");
    dout(cout << "---------------------------" << "\n");
    dout(cout << "---------------------------" << "\n");

    // Seed the random-number generator with current time so that
    // the numbers will be different every time we run.
    int seed = (int) time(NULL);     // random seed
    UNUSED(seed);

#if 0
    dout(cout  << " check do_sample() " << "\n");
    int val[4]= {12,13,14,15};
    double p[4]= {0.01,0.200,0.300,0.490};
    // draw 100 numbers from these 4 values, see myRutils.cpp
    vector<int> res = do_sample(100, 4, val, p);
    for(unsigned int i=0; i<res.size(); i++)
    {
        dout(cout  << " " << res[i]);
    }
    dout(cout  << "\n");
#endif

    dout(cout << " check rgamma() " << "\n");
    double a_shape = 1;
    double a_scale = 60;
    vector<double> a_res;
    for (int i = 0; i < 100; i++) {
        a_res.push_back(rgamma(a_shape, a_scale));
        dout(cout << " " << a_res[i]);
    }
    dout(cout << "\n");

    // test the splitting of trees
    // (NOTE: this method will fail if the same variables are present in several branches...use a data.frame instead?)
    string a_string = "0.0 last_trip_was 0.3 weather_is 0.5 fish_price_is 0.2 last_trip_was 0.8 remaining_quota_is 0.9";
    vector<string> a_split_string;
    string a_delimiter = " fish_price_is ";
    split(a_split_string, a_string, a_delimiter, splitX::no_empties);
    //	print( a_split_string );
    // second split....
    a_string = a_split_string[0];
    a_delimiter = " weather_is ";
    split(a_split_string, a_string, a_delimiter, splitX::no_empties);
    //	print( a_split_string );
    //system("PAUSE");

}


}