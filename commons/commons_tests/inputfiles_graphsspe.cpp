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

BOOST_AUTO_TEST_CASE( test_coord_dat )
{
    // Test the function fill_from_coord() from myutils.cpp

    std::istringstream is("1.0 \n"
                          "2.5\n"
                          "3.0\r\n" // Long
                          "0.5\n"
                          "3.0 \n"
                          "3.5\n"   // Lat
                          "1\n"
                          "2\n"
                          "3  "     // harbour idx
                          );

    std::vector<double> x, y;
    std::vector<int> harb;

    std::vector<double> exp_x {1.0, 2.5, 3.0};
    std::vector<double> exp_y {0.5, 3.0, 3.5};
    std::vector<int> exp_harb {1, 2, 3};
    bool r;

    r = fill_from_coord(is, x, y, harb, 3);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_x.begin(), exp_x.end(), x.begin(), x.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_y.begin(), exp_y.end(), y.begin(), y.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_harb.begin(), exp_harb.end(), harb.begin(), harb.end());
}

BOOST_AUTO_TEST_CASE( test_graph_dat )
{
    // Test the function fill_from_graph() from myutils.cpp

    std::istringstream is("1 \n"
                          "2\n"
                          "3\r\n" // from
                          "2\n"
                          "3 \n"
                          "1\n"   // To
                          "1.0\n"
                          "2.0\n"
                          "3.2  "  // Weights
                          );

    std::vector<int> f, t;
    std::vector<int> w;

    std::vector<int> exp_f {1, 2, 3};
    std::vector<int> exp_t {2, 3, 1};
    std::vector<int> exp_w {1, 2, 3};

    bool r;

    r = fill_from_graph(is, f, t, w, 3);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_f.begin(), exp_f.end(), f.begin(), f.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_t.begin(), exp_t.end(), t.begin(), t.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_w.begin(), exp_w.end(), w.begin(), w.end());
}

BOOST_AUTO_TEST_CASE (test_code_area_for_graph_points_dat)
{
    std::istringstream is("0\n0\n0\n"   // ignored
                          "0\n0\n0\n"   // ignored
                          "1 \n"
                          "2\n"
                          "3\r\n" // three infos
                            );


    std::vector<int> i;
    std::vector<int> exp_i {1,2,3};
    bool r;

    r = fill_from_code_area(is, i, 3);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_i.begin(), exp_i.end(), i.begin(), i.end());
}

BOOST_AUTO_TEST_CASE ( test_fill_from_code_marine_landscape_dat )
{
    std::istringstream is("1 \n"
                          "2\n"
                          "3\r\n" // three infos
                            );


    std::vector<int> i;
    std::vector<int> exp_i {1,2,3};
    bool r;

    r = fill_from_code_marine_landscape(is, i, 3);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_i.begin(), exp_i.end(), i.begin(), i.end());
}

BOOST_AUTO_TEST_CASE (test_metier_closure_a_graph_quarter_dat )
{
    // TODO check for lines with just one field!
    // TODO fix to load the poly id somehow?
    std::istringstream is1("5 1 1 2 3 4   \n"
                          "4 2 5 6 7 \n"
                          "1001 3 100 \r\n" // three infos
                            );


    std::vector<int> nd1 {1,2,3,4};
    std::vector<int> nd2 {5,6,7};
    std::vector<int> nd3 {100};
    std::vector<NodeBanningInfo> ban;
    bool r;

    r = read_metier_closures(is1, " ", ban);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(1, ban[0].nodeId);
    BOOST_CHECK_EQUAL_COLLECTIONS(nd1.begin(), nd1.end(), ban[0].banned.begin(), ban[0].banned.end());
    BOOST_CHECK_EQUAL(2, ban[1].nodeId);
    BOOST_CHECK_EQUAL_COLLECTIONS(nd2.begin(), nd2.end(), ban[1].banned.begin(), ban[1].banned.end());
    BOOST_CHECK_EQUAL(3, ban[2].nodeId);
    BOOST_CHECK_EQUAL_COLLECTIONS(nd3.begin(), nd3.end(), ban[2].banned.begin(), ban[2].banned.end());

    std::istringstream is2;
    std::vector<NodeBanningInfo> ban2;

    r = read_metier_closures(is2, " ", ban2);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(0, ban2.size());

    std::istringstream is3("5 x 1 2 3 4   \n"
                          "4 2 y 6 7 \n"
                          "1001 3 100 \r\n" // three infos
                            );
    r = read_metier_closures(is3, " ", ban2);
    BOOST_CHECK(!r);
}

