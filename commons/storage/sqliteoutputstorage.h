#ifndef SQLITEOUTPUTSTORAGE_H
#define SQLITEOUTPUTSTORAGE_H

#include <memory>

class VesselDefTable;

class SQLiteOutputStorage
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    SQLiteOutputStorage(std::string path);
    ~SQLiteOutputStorage() noexcept;

    void open();
    void close();

    void createAllTables();
    std::shared_ptr<VesselDefTable> getVesselDefTable() const;
};

#endif // SQLITEOUTPUTSTORAGE_H
