#include "sqliteoutputstorage.h"

#include "sqlitestorage.h"
#include "tables/vesseldeftable.h"

using namespace sqlite;

struct SQLiteOutputStorage::Impl {
    std::shared_ptr<SQLiteStorage> db;

    std::shared_ptr<VesselDefTable> mVesselDefTable;
};

SQLiteOutputStorage::SQLiteOutputStorage(std::string path)
{
    p = std::make_unique<Impl>();
    p->db = std::make_shared<SQLiteStorage>(std::move(path));
}

SQLiteOutputStorage::~SQLiteOutputStorage() noexcept = default;

void SQLiteOutputStorage::open()
{
    p->db->open();
}

void SQLiteOutputStorage::close()
{
    p->db->close();
}

void SQLiteOutputStorage::createAllTables()
{
    p->mVesselDefTable = std::make_shared<VesselDefTable>(p->db, "VesselDef");
}

std::shared_ptr<VesselDefTable> SQLiteOutputStorage::getVesselDefTable() const
{
    return p->mVesselDefTable;
}



