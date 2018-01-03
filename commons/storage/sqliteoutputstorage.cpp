#include "sqliteoutputstorage.h"

#include "sqlitestorage.h"
#include "tables/vesseldeftable.h"
#include "tables/vesselslogliketable.h"
#include "tables/popnodestable.h"
#include "tables/poptable.h"
#include "tables/vesselvmsliketable.h"
#include "tables/vesselsloglikecatchestable.h"
#include "tables/fishfarmstable.h"
#include "tables/windfarmstable.h"

#include <cassert>

#include <Vessel.h>
#include <Population.h>

using namespace sqlite;

struct SQLiteOutputStorage::Impl {
    std::shared_ptr<SQLiteStorage> db;

    std::shared_ptr<VesselDefTable> mVesselDefTable;
    std::shared_ptr<VesselsLoglikeTable> mVesselLoglikeTable;
    std::shared_ptr<VesselsLoglikeCatchesTable> mVesselLoglikeCatchesTable;
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
    p->mVesselLoglikeCatchesTable = std::make_shared<VesselsLoglikeCatchesTable> (p->db, "VesselLogLikeCatches");
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

void SQLiteOutputStorage::exportLogLike(Vessel *v, const std::vector<double> &cumul, unsigned int tstep)
{
    auto length_class =v->get_length_class();

    VesselsLoglikeTable::Log log;
    log.id = v->get_idx();
    log.tstep = tstep;
    log.tstepdep = v->get_tstep_dep();
    log.node_id = v->get_loc()->get_idx_node();
    log.metierId = -1;
    log.lastHarbour = -1;
    log.revenueAV = v->getLastTripRevenues();
    log.revenueExAV = v->getLastTripExplicitRevenues();
    log.timeAtSea = v->get_timeatsea();
    log.reasonToGoBack = v->get_reason_to_go_back();
    log.cumFuelCons = v->get_cumfuelcons();

    if(log.cumFuelCons>1)
        log.vpuf = log.revenueAV / log.cumFuelCons;

    log.fuelCost = v->get_cumfuelcons() * v->get_loc()->get_fuelprices(length_class);
    log.gav = log.revenueAV-log.fuelCost;

    log.sweptArea=v->get_sweptareathistrip()*1e6; // CAUTION: converted in m^2 for output file

    log.revenuePerSweptArea = 0;
    if(log.sweptArea>10) // i.e. at least 10 sqr meters
          log.revenuePerSweptArea=log.revenueAV/(log.sweptArea); // euro per m^2

    log.GVA = v->get_GVA();
    log.GVAPerRevenue = v->get_GVAPerRevenue();
    log.LabourSurplus = v->get_LabourSurplus();

    log.GrossProfit = v->get_GrossProfit();
    log.NetProfit = v->get_NetProfit();
    log.NetProfitMargin = v->get_NetProfitMargin();
    log.RoFTA = v->get_RoFTA();
    log.GVAPerFTE = v->get_GVAPerFTE();
    log.BER = v->get_BER();
    log.CRBER = v->get_CRBER();
    log.NetPresentValue = v->get_NetPresentValue();
    log.numTrips=v->getNumTrips();

    auto rowid = p->mVesselLoglikeTable->insertLog(log);

    for (size_t i = 0; i < cumul.size(); ++i) {
        p->mVesselLoglikeCatchesTable->insertPopulation(rowid, i, cumul);
    }
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
    p->mVesselLoglikeCatchesTable->dropAndCreate();
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

