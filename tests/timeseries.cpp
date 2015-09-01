#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <simulation.h>

using namespace displace::simulation;

BOOST_AUTO_TEST_SUITE(TimeSeries)

BOOST_AUTO_TEST_CASE( TimeSeries )
{
    Simulation *s = Simulation::instance();
    s->loadTimeSeries("data/tests/timeseries/", "");

    BOOST_CHECK(s->getTimeSeries(displace::simulation::TimeSeriesManager::Fishprice, 0, 4) != 0);
}

BOOST_AUTO_TEST_SUITE_END()
