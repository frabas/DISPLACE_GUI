//
// Created by happycactus on 2/22/20.
//

#include "RuntimeTests.h"
#include "myutils.h"
#include "myRutils.h"
#include "helpers.h"

#include <iostream>

using namespace std;

namespace test {

void myUtils()
{
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " TEST MY UTILS             " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    double MLS = 40;
    double L75 = 46.4;
    double L50 = 27.4;
    double S1 = L50 * log(3) / (L75 - L50);
    double S2 = S1 / L50;
    double result1 = simpson<myintegrand>(20, MLS, 100, S1, S2); // discards
    double result2 = simpson<myintegrand>(MLS, 70, 100, S1, S2); // landings
    dout(cout << result1 << "  " << result2 << endl);


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
    cout << result3 << "  " << result4 << endl;
}

void myRUtils()
{
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " TEST MY R UTILS           " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // Seed the random-number generator with current time so that
    // the numbers will be different every time we run.
    int seed = (int) time(NULL);     // random seed
    UNUSED(seed);

#if 0
    dout(cout  << " check do_sample() " << endl);
    int val[4]= {12,13,14,15};
    double p[4]= {0.01,0.200,0.300,0.490};
    // draw 100 numbers from these 4 values, see myRutils.cpp
    vector<int> res = do_sample(100, 4, val, p);
    for(unsigned int i=0; i<res.size(); i++)
    {
        dout(cout  << " " << res[i]);
    }
    dout(cout  << endl);
#endif

    dout(cout << " check rgamma() " << endl);
    double a_shape = 1;
    double a_scale = 60;
    vector<double> a_res;
    for (int i = 0; i < 100; i++) {
        a_res.push_back(rgamma(a_shape, a_scale));
        dout(cout << " " << a_res[i]);
    }
    dout(cout << endl);

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