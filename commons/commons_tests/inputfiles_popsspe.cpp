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

template <typename T >
std::ostream &operator << (std::ostream &stream, const std::vector<T> &c) {
    stream << "[ ";
    for (auto v : c) {
        stream << v << " ";
    }
    stream << "]";
    return stream;
}

template <typename K, typename V>
std::ostream &operator << (std::ostream &stream, const std::pair<K,V> &c) {
    stream << "(" << c.first << "," << c.second << ")";
    return stream;
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

// TODO simplify usage of fill_in_fbar_ages_min_max: Read values from files, don't expect result vector to have already the right number of values.
BOOST_AUTO_TEST_CASE ( test_spe_fbar_amin_amax_ftarget_Fpercent_TACpercent_dat )
{
    // i.e. 6 columns: F min age, F max age, LTMP ftarget, Fpercent, TACpercent, Btrigger, F-MSY value
    vector<double> res(7);
    vector<double> ex = {4,6,0.46,0,0,0,0};

    std::istringstream ss ("4 6 0.46 0 0 0 0");
    fill_in_fbar_ages_min_max(ss, res);
    BOOST_CHECK_EQUAL_COLLECTIONS(ex.begin(), ex.end(), res.begin(), res.end());
}

BOOST_AUTO_TEST_CASE ( test_overall_migration_fluxes_semester_biolsce_dat )
{
}

BOOST_AUTO_TEST_CASE ( test_spe_initial_tac_dat )
{
    vector<double> res(1);
    vector<double> ex = {11.4};

    std::istringstream ss ("11.4");
    fill_in_initial_tac(ss, res);
    BOOST_CHECK_EQUAL_COLLECTIONS(ex.begin(), ex.end(), res.begin(), res.end());
}

BOOST_AUTO_TEST_CASE ( test_spe_percent_age_per_szgroup_biolsce_dat )
{
    vector< vector<double>> res(2, vector<double>(3));
    vector< vector<double>> ex = {
        { 1, 2, 3 }, {4, 5, 6}
    };

    std::istringstream ss ("1 2 3\n4 5 6\n");
    fill_in_percent_age_per_szgroup_matrix(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[0].begin(), ex[0].end(), res[0].begin(), res[0].end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[1].begin(), ex[1].end(), res[1].begin(), res[1].end());
}

BOOST_AUTO_TEST_CASE ( test_spe_percent_szgroup_per_age_biolsce_dat )
{
    vector< vector<double>> res(2, vector<double>(3));
    vector< vector<double>> ex = {
        { 1, 2, 3 }, {4, 5, 6}
    };

    std::istringstream ss ("1 2 3\n4 5 6\n");
    fill_in_percent_szgroup_per_age_matrix(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[0].begin(), ex[0].end(), res[0].begin(), res[0].end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[1].begin(), ex[1].end(), res[1].begin(), res[1].end());
}

BOOST_AUTO_TEST_CASE ( test_spe_size_transition_matrix_biolsce_dat )
{
    vector< vector<double>> res(2, vector<double>(3));
    vector< vector<double>> ex = {
        { 1, 2, 3 }, {4, 5, 6}
    };

    std::istringstream ss ("1 2 3\n4 5 6\n");
    fill_in_growth_transition(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[0].begin(), ex[0].end(), res[0].begin(), res[0].end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[1].begin(), ex[1].end(), res[1].begin(), res[1].end());
}

BOOST_AUTO_TEST_CASE ( test_spe_SSB_R_parameters_biolsce_dat )
{
    vector<double> res(2);
    vector<double> ex = {3.3, 4.54};

    std::istringstream ss ("3.3\n4.54\n");
    fill_in_param_sr(ss, res);
    BOOST_CHECK_EQUAL_COLLECTIONS(ex.begin(), ex.end(), res.begin(), res.end());
}

BOOST_AUTO_TEST_CASE ( test_spe_stecf_oth_land_per_month_per_node_semester_dat )
{
    map<int,double> res;
    map<int,double> ex = { { 1, 0.5}, { 2, 1.5 }, {3, 2.5} };

    std::istringstream ss ("pt_graph landings\n1 0.5\n2 1.5\n3 2.5\n");
    fill_from_oth_land(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL(ex[0], res[0]);
    BOOST_CHECK_EQUAL(ex[1], res[1]);
    BOOST_CHECK_EQUAL(ex[2], res[2]);
}

BOOST_AUTO_TEST_CASE ( test_comcat_per_szgroup_done_by_hand_dat )
{
    Finder<int, int, std::multimap<int,int>> fnd;
    multimap<int,int> res;
    multimap<int,int> ex = { { 1, 5}, { 2, 6 }, {3, 7} };

    std::istringstream ss ("stock comcat_per_szgroup\n1 5\n2 6\n3 7\n");
    fill_multimap_from_specifications_i_i(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL(fnd(ex,1), fnd(res,1));
    BOOST_CHECK_EQUAL(fnd(ex,2), fnd(res,2));
    BOOST_CHECK_EQUAL(fnd(ex,3), fnd(res,3));
}

BOOST_AUTO_TEST_CASE ( test_hyperstability_param_dat )
{
    map<int,double> res;
    map<int,double> ex = { { 1, 0.5}, { 2, 1.5 }, {3, 2.5} };

    std::istringstream ss ("pop hyperstability_param\n1 0.5\n2 1.5\n3 2.5\n");
    fill_map_from_specifications_i_d(ss, res, "dummy");
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL(ex[0], res[0]);
    BOOST_CHECK_EQUAL(ex[1], res[1]);
    BOOST_CHECK_EQUAL(ex[2], res[2]);
}

BOOST_AUTO_TEST_CASE ( test_init_fecundity_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true);      // uses fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_M_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_maturity_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_pops_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_prop_migrants_pops_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_proprecru_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_init_weight_per_szgroup_biolsce_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

BOOST_AUTO_TEST_CASE ( test_percent_landings_from_simulated_vessels_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}

BOOST_AUTO_TEST_CASE ( test_species_interactions_mortality_proportion_matrix_biolsce_dat )
{
    vector< vector<double>> res(2, vector<double>(3));
    vector< vector<double>> ex = {
        { 1, 2.6, 3 }, {.4, 5, 6}
    };

    std::istringstream ss ("1 2.6 3\n.4 5 6\n");
    fill_in_species_interactions_mortality_proportion_matrix(ss, res);
    BOOST_CHECK_EQUAL(ex.size(), res.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[0].begin(), ex[0].end(), res[0].begin(), res[0].end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex[1].begin(), ex[1].end(), res[1].begin(), res[1].end());
}

BOOST_AUTO_TEST_CASE ( test_the_selected_szgroups_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_i
}

BOOST_AUTO_TEST_CASE ( test_avai_betas_semester_dat )
{
    BOOST_CHECK(true); // fill_multimap_from_specifications_i_d
}

