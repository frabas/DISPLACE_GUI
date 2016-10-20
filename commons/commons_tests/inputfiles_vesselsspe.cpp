/** @file inputfiles_graphsspe.cpp
 * @author Federico Fuga
 *
 * This file tests /graphsspe/ files format listed in the inputformats.txt specification.
 *
 * */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <readdata.h>
#include <map>

namespace {

struct Vessel {
    string names;
    double speeds;
    double fuelcons;
    double lengths;
    double vKWs;
    double carrycapacities;
    double tankcapacities;
    double nbfpingspertrips;
    double resttime_par1s;
    double resttime_par2s;
    double av_trip_duration;
    double mult_fuelcons_when_steaming;
    double mult_fuelcons_when_fishing;
    double mult_fuelcons_when_returning;
    double mult_fuelcons_when_inactive;
    VesselCalendar calendars;

    Vessel() {}

    Vessel(string names,
           double speeds,
           double fuelcons,
           double lengths,
           double vKWs,
           double carrycapacities,
           double tankcapacities,
           double nbfpingspertrips,
           double resttime_par1s,
           double resttime_par2s,
           double av_trip_duration,
           double mult_fuelcons_when_steaming,
           double mult_fuelcons_when_fishing,
           double mult_fuelcons_when_returning,
           double mult_fuelcons_when_inactive,
           int westart,int  weend, int wrkstart, int wrkend
           ) :
        names(names),
        speeds(speeds),
        fuelcons(fuelcons),
        lengths(lengths),
        vKWs(vKWs),
        carrycapacities(carrycapacities),
        tankcapacities(tankcapacities),
        nbfpingspertrips(nbfpingspertrips),
        resttime_par1s(resttime_par1s),
        resttime_par2s(resttime_par2s),
        av_trip_duration(av_trip_duration),
        mult_fuelcons_when_steaming(mult_fuelcons_when_steaming),
        mult_fuelcons_when_fishing(mult_fuelcons_when_fishing),
        mult_fuelcons_when_returning(mult_fuelcons_when_returning),
        mult_fuelcons_when_inactive(mult_fuelcons_when_inactive),
        calendars(westart,weend,wrkstart,wrkend) {
    }


    friend bool operator == (const Vessel &v1, const Vessel &v2) {
        return
            v1.names == v2.names &&
            v1.speeds == v2.speeds &&
            v1.fuelcons == v2.fuelcons &&
            v1.lengths == v2.lengths &&
            v1.vKWs == v2.vKWs &&
            v1.carrycapacities == v2.carrycapacities &&
            v1.tankcapacities == v2.tankcapacities &&
            v1.nbfpingspertrips == v2.nbfpingspertrips &&
            v1.resttime_par1s == v2.resttime_par1s &&
            v1.resttime_par2s == v2.resttime_par2s &&
            v1.av_trip_duration == v2.av_trip_duration &&
            v1.mult_fuelcons_when_steaming == v2.mult_fuelcons_when_steaming &&
            v1.mult_fuelcons_when_fishing == v2.mult_fuelcons_when_fishing &&
            v1.mult_fuelcons_when_returning == v2.mult_fuelcons_when_returning &&
            v1.mult_fuelcons_when_inactive == v2.mult_fuelcons_when_inactive &&
            v1.calendars == v2.calendars;
    }

    friend bool operator != (const Vessel &v1, const Vessel &v2) {
        return !(v1==v2);
    }

    friend std::ostream &operator << (std::ostream &stream, const VesselCalendar &cal) {
        stream << "[" << cal.weekEndStartDay << "," << cal.weekEndEndDay << "," <<
                  cal.workStartHour << "," << cal.workEndHour << "]";
        return stream;
    }

    friend std::ostream &operator << (std::ostream &stream, const Vessel &v1) {
        stream << v1.names;
        return stream;
    }
};

bool loadVessels(std::istream &stream, std::vector<Vessel> &vessels) {
    vector<string> names;
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
    vector<VesselCalendar> calendars;

    if (!fill_from_vessels_specifications (stream,
                                           names,
                                           speeds,
                                           fuelcons,
                                           lengths,
                                           vKWs,
                                           carrycapacities,
                                           tankcapacities,
                                           nbfpingspertrips,
                                           resttime_par1s,
                                           resttime_par2s,
                                           av_trip_duration,
                                           mult_fuelcons_when_steaming,
                                           mult_fuelcons_when_fishing,
                                           mult_fuelcons_when_returning,
                                           mult_fuelcons_when_inactive,
                                           calendars))
        return false;

    for (size_t i = 0; i < names.size(); ++i) {
        Vessel v;
        v.names = names[i];
        v.speeds = speeds[i];
        v.fuelcons = fuelcons[i];
        v.lengths = lengths[i];
        v.vKWs = vKWs[i];
        v.carrycapacities = carrycapacities[i];
        v.tankcapacities = tankcapacities[i];
        v.nbfpingspertrips = nbfpingspertrips[i];
        v.resttime_par1s = resttime_par1s[i];
        v.resttime_par2s = resttime_par2s[i];
        v.av_trip_duration = av_trip_duration[i];
        v.mult_fuelcons_when_steaming = mult_fuelcons_when_steaming[i];
        v.mult_fuelcons_when_fishing = mult_fuelcons_when_fishing[i];
        v.mult_fuelcons_when_returning = mult_fuelcons_when_returning[i];
        v.mult_fuelcons_when_inactive = mult_fuelcons_when_inactive[i];
        v.calendars = calendars[i];

        vessels.emplace_back(v);
    }

    return true;
}

}

BOOST_AUTO_TEST_SUITE ( vesselsspe )


BOOST_AUTO_TEST_CASE( test_cpue_per_stk_on_nodes_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_possible_metiers_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}

BOOST_AUTO_TEST_CASE( test_freq_possible_metiers_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_gscale_cpue_per_stk_on_nodes_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_gshape_cpue_per_stk_on_nodes_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_fuel_price_per_vessel_size_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_initial_fishing_credits_per_vid_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_s
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_betas_semester_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_d
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_features_quarter_dat )
{
    std::string teststring;
    teststring = "DNK000001391|10|34.18397|15|128|23170|7565|10|0.4485|336.7618|20|1|0|1.1|0.15|5|6|5|22\n"
                 "DNK0000012s1|30|34.248397|5|18|270|75|1|0.5|3.78|2|1|0|0.1|0.1522|5|6|5|22\n";

    std::istringstream ss (teststring);

    std::vector<Vessel> exp_ss = {
        Vessel("DNK000001391",10,34.18397,15,128,23170,7565,10,0.4485,336.7618,20,1,0,1.1,0.15,5,6,5,22),
        Vessel("DNK0000012s1",30,34.248397,5,18,270,75,1,0.5,3.78,2,1,0,0.1,0.1522,5,6,5,22)
    };

    std::vector<Vessel> r_ss;

    bool r = loadVessels(ss, r_ss);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_ss.begin(), exp_ss.end(), r_ss.begin(), r_ss.end());

    // Check for errors. Errors in double values
    teststring = "DNK000001391|1c|34.18397|15|128|23170|7565|10|0.4485|336.7618|20|1|0|1.1|0.15|5|6|5|22\n";
    ss.str(teststring);
    ss.clear();
    r = loadVessels(ss,r_ss);
    BOOST_CHECK(!r);

    // Check for errors. Errors in the number of fields
    teststring = "DNK000001391|1c|34.18397|15|128|23170|\n";
    ss.str(teststring);
    ss.clear();
    r = loadVessels(ss,r_ss);
    BOOST_CHECK(!r);

    // Check for errors: Missing newline should be catched
    teststring = "DNK0000011|0|34.18397|15|128|23170|7565|10|0.4485|336.7618|20|1|0|1.1|0.15|5|6|5|22";
    r_ss.clear();
    exp_ss = {
            Vessel("DNK0000011",0,34.18397,15,128,23170,7565,10,0.4485,336.7618,20,1,0,1.1,0.15,5,6,5,22)
        };
    ss.str(teststring);
    ss.clear();
    r = loadVessels(ss,r_ss);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(exp_ss.size(), r_ss.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_ss.begin(), exp_ss.end(), r_ss.begin(), r_ss.end());
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_fgrounds_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_i
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_freq_fgrounds_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_d
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_harbours_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_i
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_freq_harbours_quarter_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_d
}

BOOST_AUTO_TEST_CASE( test_vesselsspe_percent_tacs_per_pop_semester_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_s_d
}

BOOST_AUTO_TEST_SUITE_END()
