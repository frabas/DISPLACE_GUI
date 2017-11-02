#include "sqliteresultsstorage.h"
#include "sqlitetable.h"

#include "utils/make_unique.h"

SQLiteResultsStorage::SQLiteResultsStorage(std::string path)
{
    dbPath = std::move(path);
}

SQLiteResultsStorage::~SQLiteResultsStorage() noexcept
{
    if (mDb != nullptr) {
        sqlite3_close(mDb);
    }
}


bool SQLiteResultsStorage::open()
{
    auto r = sqlite3_open(dbPath.c_str(), &mDb);
    if (r != SQLITE_OK) {
        throw SQLiteException(mDb);
    }
    return true;
}

bool SQLiteResultsStorage::close()
{
    sqlite3_close(mDb);
    mDb = nullptr;
    return true;
}

bool SQLiteResultsStorage::addTable(std::shared_ptr<SQLiteTable> table)
{
    std::string qry {"SELECT name FROM sqlite_master WHERE type='table' AND name=?1;"};
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(mDb, qry.c_str(), qry.size(), &stmt, nullptr) != SQLITE_OK)
        throw SQLiteException(mDb);

    auto name = table->name();
    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), 0) != SQLITE_OK)
        throw SQLiteException(mDb);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        // table doesn't exist.
        if (!table->create()) {
            return false;
        }
    }

    tables.push_back(std::move(table));
    return true;
}

sqlite3 *SQLiteResultsStorage::handle()
{
    return mDb;
}
