#include "sqlitetable.h"

#include "sqlitestorage.h"
#include <sqlite3.h>

#include <sstream>
#include <iostream>

using namespace sqlite;

class SQLiteTable::Statement {
    std::shared_ptr<SQLiteStorage> mDb;
    sqlite3_stmt *stmt = nullptr;
public:
    explicit Statement(std::shared_ptr<SQLiteStorage> db, std::string sql)
    : mDb(db)
    {
        auto r = sqlite3_prepare_v2(mDb->handle(), sql.c_str(), -1, &stmt, nullptr);
        if (r != SQLITE_OK)
            throw SQLiteException(mDb->handle());
    }

    sqlite3_stmt *handle() const { return stmt; }
    std::shared_ptr<SQLiteStorage> db() const { return mDb; }
};

SQLiteTable::SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : mdb(db), mName(name)
{

}

std::shared_ptr<SQLiteStorage> SQLiteTable::db()
{
    auto db = mdb.lock();

    if (db == nullptr)
        throw std::logic_error("Operation on an orphaned Table!");
    return db;
}

bool SQLiteTable::createFromSQLString(std::string query)
{
    auto db = this->db();

    sqlite3_stmt *s;
    if (sqlite3_prepare(db->handle(), query.c_str(), query.size(), &s, nullptr) != SQLITE_OK)
        throw SQLiteException(db->handle());

    if (sqlite3_step(s) != SQLITE_DONE) {
        throw SQLiteException(db->handle());
    }

    return true;
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, int value)
{
    auto r = sqlite3_bind_int(stmt->handle(), idx, value);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, std::string value)
{
    auto r = sqlite3_bind_text(stmt->handle(), idx, value.c_str(), value.size(), SQLITE_TRANSIENT);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, double value)
{
    auto r = sqlite3_bind_double(stmt->handle(), idx, value);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

std::shared_ptr<SQLiteTable::Statement> SQLiteTable::newStatement(std::string query)
{
    auto s = std::make_shared<Statement>(mdb.lock(), query);

    return s;
}

bool SQLiteTable::execute(SQLiteTable::Statement *stmt)
{
    auto r = sqlite3_step(stmt->handle());
    if (r != SQLITE_DONE)
        throw SQLiteException(stmt->db()->handle());
    return true;
}

SQLiteTable::~SQLiteTable() noexcept = default;

