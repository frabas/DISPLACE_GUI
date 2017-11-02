#ifndef SQLITERESULTSTORAGE_H
#define SQLITERESULTSTORAGE_H

#include <sqlite3.h>

#include <string>
#include <memory>
#include <stdexcept>
#include <list>

class SQLiteTable;

class SQLiteException : public std::runtime_error {
    std::string mErrmsg;
    int mCode;
public:
    explicit SQLiteException(sqlite3 *db) : std::runtime_error ("") {
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

class SQLiteStorage
{
    std::string dbPath;
    std::list<std::shared_ptr<SQLiteTable>> tables;

    sqlite3 *mDb = nullptr;
public:
    SQLiteStorage(std::string path);
    ~SQLiteStorage() noexcept;

    bool open();
    bool close();

    bool addTable (std::shared_ptr<SQLiteTable>);

    sqlite3 *handle();
};

#endif // SQLITERESULTSTORAGE_H
