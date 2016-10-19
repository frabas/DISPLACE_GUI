/** @file inputfiles_benthosspe.cpp
 * @author Federico Fuga
 *
 * This file tests /benthosspe/ files format listed in the inputformats.txt specification.
 *
 * */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <readdata.h>
#include <map>

BOOST_AUTO_TEST_SUITE ( benthosspe )


BOOST_AUTO_TEST_CASE( test_estimates_biomass_per_cell_per_funcgr_per_landscape_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_SUITE_END()
