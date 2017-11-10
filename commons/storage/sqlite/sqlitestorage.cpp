#include "sqlitestorage.h"
#include "sqlitetable.h"

using namespace sqlite;

SQLiteStorage::SQLiteStorage(std::string path)
{
    dbPath = std::move(path);
}

SQLiteStorage::~SQLiteStorage() noexcept
{
    if (mDb != nullptr) {
        sqlite3_close(mDb);
    }
}


bool SQLiteStorage::open()
{
    auto r = sqlite3_open(dbPath.c_str(), &mDb);
    if (r != SQLITE_OK) {
        throw SQLiteException(mDb);
    }
    return true;
}

bool SQLiteStorage::close()
{
    sqlite3_close(mDb);
    mDb = nullptr;
    return true;
}

sqlite3 *SQLiteStorage::handle()
{
    return mDb;
}

bool SQLiteStorage::dropTable(std::string table)
{
    std::ostringstream ss;
    ss << "DROP TABLE " << table << ";";
    auto r = sqlite3_exec(mDb, ss.str().c_str(), nullptr, nullptr, nullptr);
    if (r != SQLITE_OK)
        throw SQLiteException(mDb);

    return true;
}

bool SQLiteStorage::tableExists(std::string table)
{
    sqlite3_stmt *stmt;
    auto r = sqlite3_prepare_v2(mDb, "SELECT name FROM sqlite_master WHERE type='table' AND name=?;", -1, &stmt, nullptr);
    SQLiteException::throwIfNotOk(r,mDb);

    r = sqlite3_bind_text(stmt, 1, table.c_str(), table.length(), SQLITE_TRANSIENT);
    SQLiteException::throwIfNotOk(r,mDb);

    r = sqlite3_step(stmt);
    if (r == SQLITE_DONE) {
        return false;
    } else if (r != SQLITE_ROW) {
        SQLiteException::throwIfNotOk(r, mDb);
    }
    return true;
}
