#ifndef SQLITERESULTSTORAGE_H
#define SQLITERESULTSTORAGE_H

#include <string>
#include <memory>

#include <stdexcept>

class SQLiteResultsStorage
{
    class Impl;
    std::unique_ptr<Impl> p;
public:
    SQLiteResultsStorage(std::string path);
    ~SQLiteResultsStorage() noexcept;

    bool open();
    bool close();

    class Exception : public std::runtime_error { using std::runtime_error::runtime_error; };
};

#endif // SQLITERESULTSTORAGE_H
