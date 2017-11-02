#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "storage/sqlitestorage.h"
#include "storage/tables/vesseldeftable.h"

BOOST_AUTO_TEST_CASE (test_sqlite_table)
{
    auto db = std::make_shared<SQLiteStorage>("output.db");
    BOOST_CHECK(db->open());

    auto table = std::make_shared<VesselDefTable>(db, "VesselsDefs");
    BOOST_CHECK(db->addTable(table));


}
