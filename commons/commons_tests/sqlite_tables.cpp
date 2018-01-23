#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "sqlitestorage.h"
#include "sqlitetable.h"
#include "sqlitefielddef.h"

#include "storage/tables/vesselsloglikecatchestable.h"
#include "storage/tables/vesselvmsliketable.h"

using namespace sqlite;

BOOST_AUTO_TEST_CASE (sqlite_tables)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();

    {
        auto table = std::make_shared<VesselsLoglikeCatchesTable>(db, "catches");
        BOOST_NOEXCEPT(table->dropAndCreate());

        std::vector<double> catches { 1.0, 2.0, 3.0 };
        std::vector<double> discards { 1.0, 2.0, 3.0 };

        BOOST_NOEXCEPT(table->insert(1, 0, catches, discards));
        BOOST_NOEXCEPT(table->insert(1, 1, catches, discards));
        BOOST_NOEXCEPT(table->insert(1, 2, catches, discards));
    }

    {
        auto table = std::make_shared<VesselVmsLikeTable>(db, "vms");
        BOOST_NOEXCEPT(table->dropAndCreate());

        VesselVmsLikeTable::Log l1 { 0, 0, 1,  1.0, 2.0, 180.0, 200, 1 };
        BOOST_NOEXCEPT(table->insertLog(l1));
    }
}


