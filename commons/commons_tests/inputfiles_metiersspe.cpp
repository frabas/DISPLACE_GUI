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

BOOST_AUTO_TEST_SUITE ( metierssspe )


BOOST_AUTO_TEST_CASE( test_loss_after_one_passage_per_landscape_per_func_group_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_metier_selectivity_per_stock_ogives_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_combined_met_types_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}

BOOST_AUTO_TEST_CASE( test_met_target_names_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_metier_fspeed_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_metier_gear_widths_model_type_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_s
}

BOOST_AUTO_TEST_CASE( test_metier_gear_widths_param_a_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_metier_gear_widths_param_b_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE( test_metier_names_dat )
{
    // Not Used?
}

BOOST_AUTO_TEST_CASE( test_metierspe_betas_semester_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}


BOOST_AUTO_TEST_CASE( test_metierspe_mls_cat_semester_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}


BOOST_AUTO_TEST_CASE( test_percent_revenue_completenesses_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}

BOOST_AUTO_TEST_SUITE_END()
