#include "sqliteresultsstorage.h"
#include "sqlitetable.h"

#include "utils/make_unique.h"

#include <sqlite3.h>

#include <list>

namespace {
class SQLiteException : public SQLiteResultsStorage::Exception {
    std::string mErrmsg;
    int mCode;
public:
    explicit SQLiteException(sqlite3 *db) : Exception ("") {
        mErrmsg = sqlite3_errmsg(db);
        mCode = sqlite3_errcode(db);
    }

    const char *what() const noexcept override
    {
        return mErrmsg.c_str();
    }

    int code() const {
        return mCode;
    }
};
}

class SQLiteResultsStorage::Impl {
public:
    std::string dbPath;
    std::list<std::shared_ptr<SQLiteTable>> tables;

    sqlite3 *mDb = nullptr;

    ~Impl() {
        if (mDb != nullptr) {
            sqlite3_close(mDb);
        }
    }

    void open() {
        auto r = sqlite3_open(dbPath.c_str(), &mDb);
        if (r != SQLITE_OK) {
            throw SQLiteException(mDb);
        }
    }

    void close() {
        sqlite3_close(mDb);
        mDb = nullptr;
    }
};

SQLiteResultsStorage::SQLiteResultsStorage(std::string path)
    : p(utils::make_unique<Impl>())
{
    p->dbPath = std::move(path);
}

SQLiteResultsStorage::~SQLiteResultsStorage() noexcept = default;

bool SQLiteResultsStorage::open()
{
    p->open();
    return true;
}

bool SQLiteResultsStorage::close()
{
    p->close();
    return true;
}

bool SQLiteResultsStorage::addTable(std::shared_ptr<SQLiteTable> table)
{
    std::string qry {"SELECT name FROM sqlite_master WHERE type='table' AND name='?';"};
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(p->mDb, qry.c_str(), qry.size(), &stmt, nullptr) != SQLITE_OK)
        throw SQLiteException(p->mDb);

    auto name = table->name();
    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), 0) != SQLITE_OK)
        throw SQLiteException(p->mDb);

    if (sqlite3_step(stmt) == SQLITE_ERROR) {
        // table doesn't exist.
        if (!table->create()) {
            return false;
        }
    }

    p->tables.push_back(std::move(table));
    return true;
}

sqlite3 *SQLiteResultsStorage::handle()
{
    return p->mDb;
}
