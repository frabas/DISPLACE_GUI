#include "sqliteoutputstorage.h"

#include "sqlitestorage.h"
#include "tables/vesseldeftable.h"
#include "tables/vesselslogliketable.h"
#include "tables/popnodestable.h"
#include "tables/poptable.h"
#include "tables/vesselvmsliketable.h"
#include "tables/fishfarmstable.h"
#include "tables/windfarmstable.h"

#include <cassert>

using namespace sqlite;

struct SQLiteOutputStorage::Impl {
    std::shared_ptr<SQLiteStorage> db;

    std::shared_ptr<VesselDefTable> mVesselDefTable;
    std::shared_ptr<VesselsLoglikeTable> mVesselLoglikeTable;
    std::shared_ptr<VesselVmsLikeTable> mVesselVmslikeTable;
    std::shared_ptr<PopNodesTable> mPopNodesTable;
    std::shared_ptr<PopTable> mPopTable;
    std::shared_ptr<FishfarmsTable> mFishfarmsTable;
    std::shared_ptr<WindfarmsTable> mWindmillsTable;
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

    p->mVesselDefTable = std::make_shared<VesselDefTable>(p->db, "VesselDef");
    p->mVesselLoglikeTable = std::make_shared<VesselsLoglikeTable>(p->db, "VesselLogLike");
    p->mVesselVmslikeTable = std::make_shared<VesselVmsLikeTable>(p->db, "VesselVmsLike");
    p->mPopNodesTable = std::make_shared<PopNodesTable>(p->db, "PopNodes");
    p->mPopTable = std::make_shared<PopTable>(p->db, "PopValues");
    p->mFishfarmsTable = std::make_shared<FishfarmsTable>(p->db, "Fishfarms");
    p->mWindmillsTable = std::make_shared<WindfarmsTable>(p->db, "Windmills");
}

void SQLiteOutputStorage::close()
{
    p->db->close();
}

void SQLiteOutputStorage::startDayLoop()
{
    assert(p->db->startTransaction());
}

void SQLiteOutputStorage::endDayLoop()
{
    assert(p->db->commitTransaction());
}

std::shared_ptr<SQLiteStorage> SQLiteOutputStorage::getDb() const
{
    return p->db;
}

void SQLiteOutputStorage::exportFishfarmLog(Fishfarm *fishfarm, int tstep)
{
    p->mFishfarmsTable->exportFishfarmLog(fishfarm, tstep);
}

void SQLiteOutputStorage::exportWindmillsLog(Windmill *windmill, int tstep)
{
    p->mWindmillsTable->exportWindmillData(windmill, tstep);
}

void SQLiteOutputStorage::createAllTables()
{
    p->mVesselDefTable->dropAndCreate();
    p->mVesselLoglikeTable->dropAndCreate();
    p->mVesselVmslikeTable->dropAndCreate();
    p->mPopNodesTable->dropAndCreate();
    p->mPopTable->dropAndCreate();
    p->mFishfarmsTable->dropAndCreate();
    p->mWindmillsTable->dropAndCreate();
}

void SQLiteOutputStorage::createAllIndexes()
{
    p->mVesselDefTable->createIndex();
}

std::shared_ptr<VesselDefTable> SQLiteOutputStorage::getVesselDefTable() const
{
    return p->mVesselDefTable;
}

std::shared_ptr<VesselsLoglikeTable> SQLiteOutputStorage::getVesselLoglikeTable() const
{
    return p->mVesselLoglikeTable;
}

std::shared_ptr<VesselVmsLikeTable> SQLiteOutputStorage::getVesselVmsLikeTable() const
{
    return p->mVesselVmslikeTable;
}

std::shared_ptr<PopNodesTable> SQLiteOutputStorage::getPopNodesTable() const
{
    return p->mPopNodesTable;
}

std::shared_ptr<PopTable> SQLiteOutputStorage::getPopTable() const
{
    return p->mPopTable;
}

std::shared_ptr<WindfarmsTable> SQLiteOutputStorage::getWindfarmTable() const
{
    return p->mWindmillsTable;
}

