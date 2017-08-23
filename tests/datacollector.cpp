#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../qtgui/historicaldatacollector.h"

BOOST_AUTO_TEST_CASE( DataCollector )
{
    HistoricalDataCollector<int> coll;
    coll.insertValue(0, 0);
    coll.insertValue(2, 2);
    coll.insertValue(10,10);

    BOOST_CHECK_EQUAL(coll.getUniqueValuesCount(), 3);
    BOOST_CHECK_EQUAL(coll.getValue(0), 0);
    BOOST_CHECK_EQUAL(coll.getValue(1), 0);
    BOOST_CHECK_EQUAL(coll.getValue(2), 2);
    BOOST_CHECK_EQUAL(coll.getValue(10), 10);
    BOOST_CHECK_EQUAL(coll.getValue(11), 10);
}
