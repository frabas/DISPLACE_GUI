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

struct Ship {
    string names;
    double imos;
    double yearbuilds;
    string flags;
    string types;
    double typecodes;
    double loas;
    double KWs;
    double breadths;
    double grosstonnages;
    double nbunits;
    double fueluses;
    double NOxEmission_gperKWhs;
    double SOxEmission_percentpertotalfuelmasss;
    double GHGEmissions;
    double PMEmissions;
    double vmaxs;
    double vcruises;
    double lane_ids;

    Ship() {}

    Ship( string names,
            double imos,
            double yearbuilds,
            string flags,
            string types,
            double typecodes,
            double loas,
            double KWs,
            double breadths,
            double grosstonnages,
            double nbunits,
            double fueluses,
            double NOxEmission_gperKWhs,
            double SOxEmission_percentpertotalfuelmasss,
            double GHGEmissions,
            double PMEmissions,
            double vmaxs,
            double vcruises,
            double lane_ids ) :
        names(names),
        imos(imos),
        yearbuilds(yearbuilds),
        flags(flags),
        types(types),
        typecodes(typecodes),
        loas(loas),
        KWs(KWs),
        breadths(breadths),
        grosstonnages(grosstonnages),
        nbunits(nbunits),
        fueluses(fueluses),
        NOxEmission_gperKWhs(NOxEmission_gperKWhs),
        SOxEmission_percentpertotalfuelmasss(SOxEmission_percentpertotalfuelmasss),
        GHGEmissions(GHGEmissions),
        PMEmissions(PMEmissions),
        vmaxs(vmaxs),
        vcruises(vcruises),
        lane_ids(lane_ids) {

    }


    friend bool operator == (const Ship &v1, const Ship &v2) {
        return
            v1.names == v2.names &&
            v1.imos == v2.imos &&
            v1.yearbuilds == v2.yearbuilds &&
            v1.flags == v2.flags &&
            v1.types == v2.types &&
            v1.typecodes == v2.typecodes &&
            v1.loas == v2.loas &&
            v1.KWs == v2.KWs &&
            v1.breadths == v2.breadths &&
            v1.grosstonnages == v2.grosstonnages &&
            v1.nbunits == v2.nbunits &&
            v1.fueluses == v2.fueluses &&
            v1.NOxEmission_gperKWhs == v2.NOxEmission_gperKWhs &&
            v1.SOxEmission_percentpertotalfuelmasss == v2.SOxEmission_percentpertotalfuelmasss &&
            v1.GHGEmissions == v2.GHGEmissions &&
            v1.PMEmissions == v2.PMEmissions &&
            v1.vmaxs == v2.vmaxs &&
            v1.vcruises == v2.vcruises &&
            v1.lane_ids == v2.lane_ids;
    }

    friend bool operator != (const Ship &v1, const Ship &v2) {
        return !(v1==v2);
    }

    friend std::ostream &operator << (std::ostream &stream, const Ship &v1) {
        stream << v1.names;
        return stream;
    }
};

bool loadShips(std::istream &stream, std::vector<Ship> &ships) {
    vector<string> names;
    vector<double> imos;
    vector<double> yearbuilds;
    vector<string> flags;
    vector<string> types;
    vector<double> typecodes;
    vector<double> loas;
    vector<double> KWs;
    vector<double> breadths;
    vector<double> grosstonnages;
    vector<double> nbunits;
    vector<double> fueluses;
    vector<double> NOxEmission_gperKWhs;
    vector<double> SOxEmission_percentpertotalfuelmasss;
    vector<double> GHGEmissions;
    vector<double> PMEmissions;
    vector<double> vmaxs;
    vector<double> vcruises;
    vector<double> lane_ids;

    bool r = fill_from_ships_specifications (stream,
            names,
            imos,
            yearbuilds,
            flags,
            types,
            typecodes,
            loas,
            KWs,
            breadths,
            grosstonnages,
            nbunits,
            fueluses,
            NOxEmission_gperKWhs,
            SOxEmission_percentpertotalfuelmasss,
            GHGEmissions,
            PMEmissions,
            vmaxs,
            vcruises,
            lane_ids
            );

    if (!r)
        return false;

    for (size_t i = 0; i < names.size(); ++i) {
        Ship v;
        v.names = names[i];
        v.imos = imos[i];
        v.yearbuilds = yearbuilds[i];
        v.flags = flags[i];
        v.types = types[i];
        v.typecodes = typecodes[i];
        v.loas = loas[i];
        v.KWs = KWs[i];
        v.breadths = breadths[i];
        v.grosstonnages = grosstonnages[i];
        v.nbunits = nbunits[i];
        v.fueluses = fueluses[i];
        v.NOxEmission_gperKWhs = NOxEmission_gperKWhs[i];
        v.SOxEmission_percentpertotalfuelmasss = SOxEmission_percentpertotalfuelmasss[i];
        v.GHGEmissions = GHGEmissions[i];
        v.PMEmissions = PMEmissions[i];
        v.vmaxs = vmaxs[i];
        v.vcruises = vcruises[i];
        v.lane_ids = lane_ids[i];

        ships.emplace_back(v);
    }

    return true;
}

}

BOOST_AUTO_TEST_SUITE ( shipsspe )

BOOST_AUTO_TEST_CASE( test_shipsspe_features_dat )
{
    std::string teststring;

    teststring = "SHIP1|1000000|2006|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1\n"
                 "SHIP2|3000000|2003|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1\n";

    std::stringstream ss;

    ss.str(teststring);
    ss.clear();
    std::vector<Ship> exp_ss = {
        Ship("SHIP1",1000000,2006,"Greece","Tanker",1,250,30000,45,60000,0,200,9,0.1,0.2,0,20,10,1),
        Ship("SHIP2",3000000,2003,"Greece","Tanker",1,250,30000,45,60000,0,200,9,0.1,0.2,0,20,10,1)
    };

    std::vector<Ship> r_ss;

    bool r = loadShips(ss, r_ss);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_ss.begin(), exp_ss.end(), r_ss.begin(), r_ss.end());

    // Check for errors
    teststring = "SHIP1|1000000|20d6|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1\n";
    ss.str(teststring);
    ss.clear();
    r = loadShips(ss, r_ss);
    BOOST_CHECK(!r);

    // Wrong number of records
    teststring = "SHIP1|1000000|2006|Greece|Tanker|1|250|30000|45|60000|0\n";
    ss.str(teststring);
    ss.clear();
    r_ss.clear();
    r = loadShips(ss, r_ss);
    BOOST_CHECK(!r);

    // Wrong number of records
    teststring = "SHIP1|1000000|2006|Greece|Tanker|1|250|30000|45|60000|0|\n";
    ss.str(teststring);
    ss.clear();
    r_ss.clear();
    r = loadShips(ss, r_ss);
    BOOST_CHECK(!r);


    // missing newline, should return success as well
    teststring = "SHIP1|1000000|2006|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1\n"
                 "SHIP2|3000000|2003|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1";
    ss.str(teststring);
    ss.clear();
    r_ss.clear();
    r = loadShips(ss, r_ss);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_ss.begin(), exp_ss.end(), r_ss.begin(), r_ss.end());

    // Empty lines, should return success as well
    teststring = "\n"
                 "SHIP1|1000000|2006|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1\n\n\n"
                 "SHIP2|3000000|2003|Greece|Tanker|1|250|30000|45|60000|0|200|9|0.1|0.2|0|20|10|1";
    ss.str(teststring);
    ss.clear();
    r_ss.clear();
    r = loadShips(ss, r_ss);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_ss.begin(), exp_ss.end(), r_ss.begin(), r_ss.end());
}

BOOST_AUTO_TEST_CASE( test_shipsspe_lanes_lat_dat )
{
    // The function read_shiplanes_lat(string,string) calls fill_multimap_from_specifications_i_d
    // that is tested in files.cpp.
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test_shipsspe_lanes_lon_dat )
{
    // The function read_shiplanes_lon(string,string) calls fill_multimap_from_specifications_i_d
    // that is tested in files.cpp.
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
