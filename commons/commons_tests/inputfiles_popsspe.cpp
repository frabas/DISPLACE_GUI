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

namespace {
    template <typename KEY, typename VALUE, typename MAP >
    class Finder {
    public:
        VALUE operator () (const MAP &map, KEY key) {
            auto it = map.find(key);
            if (it != map.end())
                return it->second;
            std::ostringstream ss;
            ss << "Key not found: " << key;
            throw std::out_of_range(ss.str());
        }
    };

    template <typename KEY, typename VALUE, typename MAP>
    void print_mmap(ostream &out, const MAP &mmap) {
        for (auto itr : mmap) {
            out << itr.first << "," <<itr.second << "\n";
        }
    }
}

/// @BUG check and fix for ending newline!
BOOST_AUTO_TEST_CASE ( test_ctrysspe_relative_stability_semester_dat )
{
    std::istringstream is("ctry percent\n"
                          "DEN 33\n"
                          "DEU 33\n"
                          "SWE 33");

    std::map<std::string,double> info;
    Finder<std::string,double, std::map<std::string,double> > find_id;

    fill_from_relative_stability(is, info);
    BOOST_CHECK_EQUAL(info.size(), 3);
    BOOST_CHECK_CLOSE(find_id(info,"DEN"), 33.0f, 0.1);
    BOOST_CHECK_CLOSE(find_id(info,"DEU"), 33.0f, 0.1);
    BOOST_CHECK_CLOSE(find_id(info,"SWE"), 33.0f, 0.1);
}


BOOST_AUTO_TEST_CASE ( test_spe_fbar_amin_amax_ftarget_Fpercent_TACpercent_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_overall_migration_fluxes_semester_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_initial_tac_dat )
{
}



BOOST_AUTO_TEST_CASE ( test_spe_percent_age_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_percent_szgroup_per_age_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_size_transition_matrix_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_SSB_R_parameters_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_stecf_oth_land_per_month_per_node_semester_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_comcat_per_szgroup_done_by_hand_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_hyperstability_param_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_fecundity_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_M_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_maturity_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_pops_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_prop_migrants_pops_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_proprecru_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_init_weight_per_szgroup_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_percent_landings_from_simulated_vessels_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_species_interactions_mortality_proportion_matrix_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_the_selected_szgroups_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_avai_betas_semester_dat )
{
}

