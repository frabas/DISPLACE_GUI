#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "storage/sqlitestorage.h"
#include "storage/sqlitetable.h"
#include "storage/sqlitefielddef.h"

BOOST_AUTO_TEST_CASE (test_sqlite_table_creation)
{
    auto db = std::make_shared<SQLiteStorage>("output.db");
    BOOST_CHECK(db->open());

    auto testTable = std::make_tuple(
        makeFieldDef("Id", FieldType::Integer), //FieldAttribute::AutoIncrement),
        makeFieldDef("name", FieldType::Text),
        makeFieldDef("x", FieldType::Real),
        makeFieldDef("y", FieldType::Real)
    );

    auto table = SQLiteTable::create(db, "sample", testTable);

    struct Record {
        int id;
        std::string name;
        float x,y;
    };

    Record record = {
        0, "first", 1.1, -1.98
    };

    table.insert (testTable, std::make_tuple(record.id, record.name, record.x, record.y));
}
