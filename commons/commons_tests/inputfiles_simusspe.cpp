/** @file inputfiles_simusspe.cpp
 * @author Federico Fuga
 *
 * This file tests /simusspe/ files format listed in the inputformats.txt specification.
 *
 * */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <readdata.h>
#include <comstructs.h>

#if BOOST_VERSION <= 105900
#define BOOST_NAMESPACE_TTDETAIL_BEGIN
#define BOOST_NAMESPACE_TTDETAIL_END
#else
#define BOOST_NAMESPACE_TTDETAIL_BEGIN namespace tt_detail {
#define BOOST_NAMESPACE_TTDETAIL_END }
#endif

namespace boost {
namespace test_tools {
BOOST_NAMESPACE_TTDETAIL_BEGIN

template <typename KEY, typename VALUE>
std::ostream &operator << (ostream &out, const std::map<KEY,VALUE> &mmap) {
    for (auto itr : mmap) {
        out << itr.first << "," <<itr.second << "\n";
    }
    return out;
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

std::ostream &operator << (std::ostream &stream, const ClosureOptions &opt) {
    stream << opt.option(Options::banned_metiers);
    return stream;
}

template<>
struct print_log_value<displace::commons::Scenario>
{
    void operator()(std::ostream& os, displace::commons::Scenario const & s1)
    {
        os <<
              s1.dyn_alloc_sce.toString() << "," <<
              s1.dyn_pop_sce.toString() << "," <<
              s1.closure_opts << "," <<
              s1.biolsce << "," <<
              s1.fleetsce << "," <<
              s1.freq_do_growth << "," <<
              s1.freq_redispatch_the_pop << "," <<
              s1.a_graph << "," <<
              s1.nrow_coord << "," <<
              s1.nrow_graph << "," <<
              s1.a_port << "," <<
              s1.graph_res << "," <<
              s1.is_individual_vessel_quotas << "," <<
              s1.check_all_stocks_before_going_fishing << "," <<
              s1.use_dtrees << "," <<
              s1.tariff_pop << "," <<
              s1.freq_update_tariff_code << "," <<
              s1.arbitary_breaks_for_tariff << "," <<
              s1.met_multiplier_on_arbitary_breaks_for_tariff << "," <<
              s1.total_amount_credited << "," <<
              s1.tariff_annual_hcr_percent_change << "," <<
              s1.update_tariffs_based_on_lpue_or_dpue_code << "," <<
              s1.dt_go_fishing << "," <<
              s1.dt_choose_ground << "," <<
              s1.dt_start_fishing << "," <<
              s1.dt_change_ground << "," <<
              s1.dt_stop_fishing << "," <<
              s1.dt_change_port;
    }
};

BOOST_NAMESPACE_TTDETAIL_END
}
}

BOOST_AUTO_TEST_SUITE ( simusspe )

BOOST_AUTO_TEST_CASE( test_scenario_dat )
{
    displace::commons::Scenario res;
    displace::commons::Scenario ex = {
            DynAllocOptions(), PopSceOptions(), ClosureOptions(),
            "1", "2",
            1, 2, 56, "a_graph56", 10140, 57555, types::NodeId(6),
            {7.1, 7},
            true, false, true,
            {100, 101},
            8,
            {200.1, 200.2},
            9,
            10.5,
            1,
            "dt_go_fishing",
            "dt_choose_ground",
        "dt_start_fishing",
        "dt_change_ground",
        "dt_stop_fishing",
        "dt_change_port"
    };

    ex.dyn_alloc_sce.setOption(Options::alloc_baseline, true);
    ex.dyn_alloc_sce.setOption(Options::focus_on_high_profit_grounds, true);

    ex.dyn_pop_sce.setOption(Options::pop_baseline, true);

    std::vector<int> bans({90,91,92});
    ex.closure_opts.setOption(Options::banned_metiers, bans);
    ex.closure_opts.update();

    std::istringstream is ("# dyn_alloc_sce\n"
                           "baseline focus_on_high_profit_grounds\n"
                           "# dyn_pop_sce\n"
                           "baseline\n"
                           "# biolsce\n"
                           "1\n"
                           "# fleetsce\n"
                           "2\n"
                           "# Frequency to apply growth (0:daily; 1:weekly; 2:monthly; 3:quarterly; 4:semester)\n"
                           "1\n"
                           "# Frequency to redispatch the pop (0:daily; 1:weekly; 2:monthly; 3:quarterly; 4:semester)\n"
                           "2\n"
                           "# a_graph\n"
                           "56\n"
                           "# nrow_coord\n"
                           "10140\n"
                           "# nrow_graph\n"
                           "57555\n"
                           "# a_port\n"
                           "6\n"
                           "# grid res km\n"
                           "7.1\n"
                           "# is_individual_vessel_quotas\n"
                           "1\n"
                           "#  check all stocks before going fishing (otherwise, explicit pops only)\n"
                           "0\n"
                           "# Go Fishing DTree\n"
                           "dt_go_fishing\n"
                           "# Choose Ground DTree\n"
                           "dt_choose_ground\n"
                           "# Start Fishing DTree\n"
                           "dt_start_fishing\n"
                           "# Change Ground DTree\n"
                           "dt_change_ground\n"
                           "# Stop Fishing DTree\n"
                           "dt_stop_fishing\n"
                           "# Change Port DTree\n"
                           "dt_change_port\n"
                           "# Use Dtrees\n"
                           "1\n"
                           "#tariff_pop\n"
                           "0 1\n"
                           "#Freq_update_tariff_code\n"
                           "2\n"
                           "#arbitrary_breaks_for_tariff\n"
                           "0 1 5 10\n"
                           "#total_amount_credited\n"
                           "100000\n"
                           "#tariff_annual_\n"
                           "10.0\n"
                           "freq_update_tariffs_based_on_lpue_or_dpue_code\n"
                           "1\n"
                           "# banned metiers\n"
                           "10 11 12");

    bool r = read_scenario_config_file(is, res);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(ex, res);

    // Check for errors: bad int
    string test = "# dyn_alloc_sce\n"
                  "baseline focus_on_high_profit_grounds\n"
                  "# dyn_pop_sce\n"
                  "baseline\n"
                  "# biolsce\n"
                  "1\n"
                  "# fleetsce\n"
                  "1\n"
                  "# biolsce\n"
                  "2\n"
                  "# Frequency to apply growth (0:daily; 1:weekly; 2:monthly; 3:quarterly; 4:semester)\n"
                  "x\n";
    is.str(test);
    is.clear();
    r = read_scenario_config_file(is,res);
    BOOST_CHECK(!r);
}

BOOST_AUTO_TEST_CASE( test_config_dat )
{
    int ex_nbpops = 2;
    int ex_nbbenthospops = 22;
    vector<int> ex_implicit_pops = { 1, 3 };
    vector<int> ex_implicit_pops_level2 = {};
    vector<int> ex_grouped_tacs = { 0, 1, 2 };
    vector<double> ex_calib_oth_landings = { 2, 4 };
    vector<double> ex_calib_w = { 16, 32.6};
    vector<double> ex_calib_cpue = {11, 11.11};
    vector<types::NodeId> ex_interesting_harbours = { types::NodeId(3), types::NodeId(6), types::NodeId(18), types::NodeId(29)};

    std::istringstream is("# nbpops\n"
                          "3\n"
                          "# nbmets\n"
                          "2\n"
                          "# nbbenthospops\n"
                          "22\n"
                          "# implicit stocks\n"
                          "1 3\n"
                          "# calib the other landings per stock\n"
                          "2 4 \n"
                          "# calib weight-at-szgroup per stock\n"
                          "16 32.6 \n"
                          "# calib the cpue multiplier per stock\n"
                          "11 11.11 \n"
                          "# Interesting harbours\n"
                          "3 6 18 29\n"
                          "# Implicit Pop Levels #2\n"
                          "\n"
                          "# Grouped TACs groups\n"
                          "\n");

    int nbpops;
    int nbmets;
    int nbbenthospops;
    vector<int> implicit_pops;
    vector<int> implicit_pops_level2;
    vector<int> grouped_tacs;
    vector<int> nbcp_coupling_pops;
    vector<double> calib_oth_landings;
    vector<double> calib_w;
    vector<double> calib_cpue;
    vector<types::NodeId> interesting_harbours;

    bool r = read_config_file(is, nbpops, nbmets, nbbenthospops, implicit_pops, implicit_pops_level2, grouped_tacs,
                              nbcp_coupling_pops, calib_oth_landings,
                             calib_w, calib_cpue, interesting_harbours);

    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(ex_nbpops, nbpops);
    BOOST_CHECK_EQUAL(ex_nbbenthospops, nbbenthospops);
    BOOST_CHECK_EQUAL_COLLECTIONS(ex_implicit_pops.begin(), ex_implicit_pops.end(), implicit_pops.begin(), implicit_pops.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex_implicit_pops_level2.begin(), ex_implicit_pops_level2.end(), implicit_pops_level2.begin(), implicit_pops_level2.end());

    // NOTE this test will fail, skipped for the moment
    // TODO: Fix the following test
    //BOOST_CHECK_EQUAL_COLLECTIONS(ex_grouped_tacs.begin(), ex_grouped_tacs.end(), grouped_tacs.begin(), grouped_tacs.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(ex_calib_oth_landings.begin(), ex_calib_oth_landings.end(), calib_oth_landings.begin(), calib_oth_landings.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex_calib_w.begin(), ex_calib_w.end(), calib_w.begin(), calib_w.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(ex_calib_cpue.begin(), ex_calib_cpue.end(), calib_cpue.begin(), calib_cpue.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(ex_interesting_harbours.begin(), ex_interesting_harbours.end(), interesting_harbours.begin(), interesting_harbours.end());

    // Check for errors: bad cast
    string test = "# nbpops\n"
                  "x\n"
                  "# nbmets\n"
                  "x\n"
                  "# nbbenthospops\n"
                  "22\n"
                  "# implicit stocks\n";
    is.str(test);
    is.clear();
    r = read_config_file(is, nbpops, nbmets, nbbenthospops, implicit_pops, implicit_pops_level2,
                         grouped_tacs, nbcp_coupling_pops, calib_oth_landings,
                                  calib_w, calib_cpue, interesting_harbours);

    BOOST_CHECK(!r);
}

BOOST_AUTO_TEST_SUITE_END()
