/** @file inputfiles_harboursspe.cpp
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

BOOST_AUTO_TEST_SUITE ( harboursspe )


BOOST_AUTO_TEST_CASE( test_quarter_each_species_per_cat_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_names_harbours_dat)
{

    BOOST_CHECK(true); // fill_multimap_from_specifications_i_s
}


BOOST_AUTO_TEST_SUITE_END()
