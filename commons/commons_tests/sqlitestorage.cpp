#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "sqlitestorage.h"
#include "sqlitetable.h"
#include "sqlitefielddef.h"

using namespace sqlite;

BOOST_AUTO_TEST_CASE (test_sqlite_table_creation)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();

    auto testTable = std::make_tuple(
        makeFieldDef("Id", FieldType::Integer()).primaryKey().autoincrement(),
        makeFieldDef("name", FieldType::Text()),
        makeFieldDef("x", FieldType::Real()),
        makeFieldDef("y", FieldType::Real())
    );

    try {
        db->dropTable("sample");
    } catch (sqlite::SQLiteException &) { // ignore errors
    }

    auto table = SQLiteTable::create(db, "sample", testTable);

    struct Record {
        int id;
        std::string name;
        double x,y;
    };

    Record record = {
        0, "first", 1.16f, -1.98f
    };

    table.insert (testTable, std::make_tuple(record.id, record.name, record.x, record.y));

    Record r2 = { 1, "uno", 0, 1000};
    table.insert (testTable, std::make_tuple(r2.id, r2.name, r2.x, r2.y));

    try {
        r2.id = 0;
        r2.name = "aaaa";
        table.insert (testTable, std::make_tuple(r2.id, r2.name, r2.x, r2.y));
    } catch (sqlite::SQLiteException &x) {
    }
}

BOOST_AUTO_TEST_CASE(test_tables)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();

    BOOST_CHECK(!db->tableExists("non-existent"));

}
