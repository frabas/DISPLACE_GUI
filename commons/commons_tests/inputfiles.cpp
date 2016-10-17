/** @file inputfiles.cpp
 * @author Federico Fuga
 *
 * This file tests all files format listed in the inputformats.txt specification.
 *
 * */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
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

    fill_from_coord(is, x, y, harb, 3);

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

    fill_from_graph(is, f, t, w, 3);

    BOOST_CHECK_EQUAL_COLLECTIONS(exp_f.begin(), exp_f.end(), f.begin(), f.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_t.begin(), exp_t.end(), t.begin(), t.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(exp_w.begin(), exp_w.end(), w.begin(), w.end());
}
