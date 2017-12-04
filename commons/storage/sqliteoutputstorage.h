#ifndef SQLITEOUTPUTSTORAGE_H
#define SQLITEOUTPUTSTORAGE_H

#include <memory>

class VesselDefTable;
class VesselsLoglikeTable;
class PopNodesTable;
class PopTable;

namespace sqlite {
class SQLiteStorage;
};

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
    std::shared_ptr<VesselsLoglikeTable> getVesselLoglikeTable() const;
    std::shared_ptr<PopNodesTable> getPopNodesTable() const;
    std::shared_ptr<PopTable> getPopTable() const;

    std::shared_ptr<sqlite::SQLiteStorage> getDb() const;
};

#endif // SQLITEOUTPUTSTORAGE_H
