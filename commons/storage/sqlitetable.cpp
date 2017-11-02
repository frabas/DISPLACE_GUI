#include "sqlitetable.h"

#include "storage/sqliteresultsstorage.h"
#include <sqlite3.h>


SQLiteTable::SQLiteTable(std::shared_ptr<SQLiteResultsStorage> db, std::string name)
    : mdb(db), mName(name)
{

}

bool SQLiteTable::create()
{
    auto db = this->db();

    std::string query {"CREATE TABLE ?1 (?2);"};

    sqlite3_stmt *s;
    if (sqlite3_prepare(db->handle(), query.c_str(), query.size(), &s, nullptr) != SQLITE_OK)
        throw SQLiteResultsStorage::Exception(db->handle());

    sqlite3_bind_text(s, 1, name(), -1, 0);
    sqlite3_bind_text(s, 2, getCreateDefinition(), -1, 0);

    if (sqlite3_step(s) != SQLITE_OK) {
        throw SQLiteResultsStorage::Exception(db->handle());
    }

    return true;
}

std::shared_ptr<SQLiteResultsStorage> SQLiteTable::db()
{
    auto db = mdb.lock();

    if (db == nullptr)
        throw std::logic_error("Operation on an orphaned Table!");
    return db;
}

SQLiteTable::~SQLiteTable() noexcept = default;

