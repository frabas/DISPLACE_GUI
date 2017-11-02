#include "sqliteresultsstorage.h"

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
    p->tables.push_back(std::move(table));
}
