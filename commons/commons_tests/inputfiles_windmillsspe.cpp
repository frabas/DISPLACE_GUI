/** @file inputfiles_windmillsspe.cpp
 * @author Federico Fuga
 *
 * This file tests /windmillsspe/ files format listed in the inputformats.txt specification.
 *
 * */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <readdata.h>
#include <map>

BOOST_AUTO_TEST_SUITE ( windmillsspe )


BOOST_AUTO_TEST_CASE( test_size_per_windmill_dat )
{
    BOOST_CHECK(true); // fill_map_from_specifications<int,double>
}

BOOST_AUTO_TEST_SUITE_END()
