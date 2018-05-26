#include "sqliteoutputstorage.h"

#include "sqlitestorage.h"
#include "sqlitefieldsop.h"
#include "sqlitestatement.h"
#include "sqlitestatementformatters.h"
#include "tables/vesseldeftable.h"
#include "tables/vesselslogliketable.h"
#include "tables/nodesdeftable.h"
#include "tables/poptable.h"
#include "tables/popdyntable.h"
#include "tables/nodesstattable.h"
#include "tables/nodestariffstattable.h"
#include "tables/funcgroupstable.h"
#include "tables/vesselvmsliketable.h"
#include "tables/vesselvmslikefpingsonlytable.h"
#include "tables/vesselsloglikecatchestable.h"
#include "tables/shipstable.h"
#include "tables/fishfarmstable.h"
#include "tables/windfarmstable.h"
#include "tables/metadatatable.h"
#include "tables/nodesenvttable.h"
#include "modelmetadataaccessor.h"

#include <cassert>

#include <Vessel.h>
#include <Metier.h>
#include <Population.h>

const int SQLiteOutputStorage::CURRENT_DB_SCHEMA_VERSION = 3;

using namespace sqlite;
using namespace displace::plot;

struct SQLiteOutputStorage::Impl {
    std::shared_ptr<SQLiteStorage> db;

    std::shared_ptr<VesselDefTable> mVesselDefTable;
    std::shared_ptr<VesselsLoglikeTable> mVesselLoglikeTable;
    std::shared_ptr<VesselsLoglikeCatchesTable> mVesselLoglikeCatchesTable;
    std::shared_ptr<VesselVmsLikeTable> mVesselVmslikeTable;
    std::shared_ptr<VesselVmsLikeFPingsOnlyTable> mVesselVmslikeFPingsOnlyTable;
    std::shared_ptr<ShipsTable> mShipsTable;
    std::shared_ptr<NodesDefTable> mNodesDefTable;
    std::shared_ptr<NodesEnvtTable> mNodesEnvtTable;
    std::shared_ptr<NodesStatTable> mNodesStatTable;
    std::shared_ptr<NodesTariffStatTable> mNodesTariffStatTable;
    std::shared_ptr<PopDynTable> mPopDynTable;
    std::shared_ptr<PopTable> mPopTable;
    std::shared_ptr<FuncGroupsTable> mFuncGroupsTable;
    std::shared_ptr<FishfarmsTable> mFishfarmsTable;
    std::shared_ptr<WindfarmsTable> mWindmillsTable;
    std::shared_ptr<MetadataTable> mMetadata;
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
    p->mMetadata = std::make_shared<MetadataTable> (p->db, "Metadata");
    p->mVesselDefTable = std::make_shared<VesselDefTable>(p->db, "VesselDef");
    p->mVesselLoglikeTable = std::make_shared<VesselsLoglikeTable>(p->db, "VesselLogLike");
    p->mVesselVmslikeTable = std::make_shared<VesselVmsLikeTable>(p->db, "VesselVmsLike");
    p->mVesselVmslikeFPingsOnlyTable = std::make_shared<VesselVmsLikeFPingsOnlyTable>(p->db, "VesselVmsFPingsOnlyLike");
    p->mVesselLoglikeCatchesTable = std::make_shared<VesselsLoglikeCatchesTable> (p->db, "VesselLogLikeCatches");
    p->mShipsTable = std::make_shared<ShipsTable> (p->db, "Ships");
    p->mNodesDefTable = std::make_shared<NodesDefTable>(p->db, "NodesDef");
    p->mNodesEnvtTable = std::make_shared<NodesEnvtTable>(p->db, "NodesEnvt");
    p->mNodesStatTable = std::make_shared<NodesStatTable>(p->db, "NodesStat");
    p->mNodesTariffStatTable = std::make_shared<NodesTariffStatTable>(p->db, "NodesTariffStat");
    p->mPopDynTable = std::make_shared<PopDynTable>(p->db, "PopDyn");
    p->mPopTable = std::make_shared<PopTable>(p->db, "PopValues");
    p->mFuncGroupsTable = std::make_shared<FuncGroupsTable>(p->db, "FuncGroups");
    p->mFishfarmsTable = std::make_shared<FishfarmsTable>(p->db, "Fishfarms");
    p->mWindmillsTable = std::make_shared<WindfarmsTable>(p->db, "Windmills");

    try {
        SQLiteStatement wal(p->db, "PRAGMA journal_mode=WAL");
        wal.execute();
    } catch (SQLiteException &x) {
        //
    }
}

void SQLiteOutputStorage::close()
{
    p->db->close();
}

int SQLiteOutputStorage::versionNumber()
{
    try {
        return p->mMetadata->getMetadataAs<int>("dbVersion");
    } catch (boost::bad_lexical_cast &) {
        return 0;
    }
}

int SQLiteOutputStorage::currentSchemaNumber()
{
    return CURRENT_DB_SCHEMA_VERSION;
}

void SQLiteOutputStorage::startDayLoop()
{
    p->db->startTransaction();
}

void SQLiteOutputStorage::endDayLoop()
{
    p->db->commitTransaction();
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

void SQLiteOutputStorage::exportEnvtNodes(int tstep, Node *node)
{
    bool r=p->mNodesEnvtTable->insert(tstep, node);
}


void SQLiteOutputStorage::exportPopNodes(int tstep, Node *node)
{
    bool r=p->mNodesStatTable->insert(tstep, node);
}

void SQLiteOutputStorage::exportTariffNodes(int tstep, Node *node)
{
    bool r=p->mNodesTariffStatTable->insert(tstep, node);
}

void SQLiteOutputStorage::exportPopStat(Population *pop, int popid, int tstep)
{
    p->mPopDynTable->insert(tstep, popid, pop);
}

void SQLiteOutputStorage::exportLogLike(Vessel *v, const std::vector<double> &cumul,const std::vector<double> &discards, unsigned int tstep)
{
    auto length_class =v->get_length_class();

    VesselsLoglikeTable::Log log;
    log.id = v->get_idx();
    log.tstep = tstep;
    log.tstepdep = v->get_tstep_dep();
    log.node_id = v->get_loc()->get_idx_node();
    log.metierId = v->get_metier()->get_name();
    log.lastHarbour = v->get_loc()->get_idx_node().toIndex();
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
        p->mVesselLoglikeCatchesTable->insertPopulation(rowid, i, cumul, discards);
    }
}

void SQLiteOutputStorage::exportCalendar(const std::vector<int> &tsteps_months, const std::vector<int> &tsteps_quarters, const std::vector<int> &tsteps_semesters, const std::vector<int> &tsteps_years)
{
    ModelMetadataAccessor a(metadata());

    a.setVectorOfInt("months", tsteps_months);
    a.setVectorOfInt("quarters", tsteps_quarters);
    a.setVectorOfInt("semesters", tsteps_semesters);
    a.setVectorOfInt("years", tsteps_years);
}

void SQLiteOutputStorage::exportShip(int tstep, Ship *ship)
{
    p->mShipsTable->exportShipsIndivators(tstep, ship);
}

void SQLiteOutputStorage::importCalendar(std::vector<int> &tsteps_months, std::vector<int> &tsteps_quarters, std::vector<int> &tsteps_semesters, std::vector<int> &tsteps_years)
{
    ModelMetadataAccessor a(metadata());

    tsteps_months = a.getVectorOfInt("months");
    tsteps_quarters = a.getVectorOfInt("quarters");
    tsteps_semesters = a.getVectorOfInt("semesters");
    tsteps_years = a.getVectorOfInt("years");
}

TimelineData SQLiteOutputStorage::getVesselLoglikeDataByNation(NationsStat stattype, string nation, Operation op)
{
    bool isAggregate = false;
    if (stattype == NationsStat::Catches || stattype == NationsStat::Discards) {
        isAggregate = true;
    }

    FieldDef<FieldType::Real> f("");
    switch (stattype) {
    case NationsStat::Catches:
        f = p->mVesselLoglikeCatchesTable->fldCatches;
        break;
    case NationsStat::Discards:
        f = p->mVesselLoglikeCatchesTable->fldDiscards;
        break;
    case NationsStat::Earnings:
        f = p->mVesselLoglikeTable->fldRevenueAV;
        break;
    case NationsStat::ExEarnings:
        f = p->mVesselLoglikeTable->revenueExAV;
        break;
    case NationsStat::TimeAtSea:
        f = p->mVesselLoglikeTable->timeAtSea;
        break;
    case NationsStat::Gav:
        f = p->mVesselLoglikeTable->gav;
        break;
    case NationsStat::Vpuf:
        f = p->mVesselLoglikeTable->vpuf;
        break;
    case NationsStat::SweptArea:
        f = p->mVesselLoglikeTable->sweptArea;
        break;
    case NationsStat::RevenuePerSweptArea:
        f = p->mVesselLoglikeTable->revenuePerSweptArea;
        break;
    case NationsStat::GVA:
        f = p->mVesselLoglikeTable->GVA;
        break;
    case NationsStat::GVAPerRevenue:
        f = p->mVesselLoglikeTable->GVAPerRevenue;
        break;
    case NationsStat::LabourSurplus:
        f = p->mVesselLoglikeTable->LabourSurplus;
        break;
    case NationsStat::GrossProfit:
        f = p->mVesselLoglikeTable->GrossProfit;
        break;
    case NationsStat::NetProfit:
        f = p->mVesselLoglikeTable->NetProfit;
        break;
    case NationsStat::NetProfitMargin:
        f = p->mVesselLoglikeTable->NetProfitMargin;
        break;
    case NationsStat::GVAPerFTE:
        f = p->mVesselLoglikeTable->GVAPerFTE;
        break;
    case NationsStat::RoFTA:
        f = p->mVesselLoglikeTable->RoFTA;
        break;
    case NationsStat::BER:
        f = p->mVesselLoglikeTable->BER;
        break;
    case NationsStat::CRBER:
        f = p->mVesselLoglikeTable->CRBER;
        break;
    case NationsStat::NetPresentValue:
        f = p->mVesselLoglikeTable->NetPresentValue;
        break;
    case NationsStat::numTrips:
        f = sqlite::FieldDef<sqlite::FieldType::Real>("rowId");
        break;
    default:
        throw std::runtime_error("getVesselLoglikeDataByNation case not handled.");
    }

    switch (op) {
    case Operation::Sum:
        f = sqlite::op::sum(f);
        break;
    case Operation::Average:
        f = sqlite::op::avg(f);
        break;
    case Operation::Count:
        f = sqlite::op::count(f);
        break;
    }

    auto select = sqlite::statements::Select(p->mVesselLoglikeTable->name(),
                                                    p->mVesselLoglikeTable->fldTStep,
                                                    f
                                                    );
    select.join(p->mVesselDefTable->name(), p->mVesselLoglikeTable->fldId, p->mVesselDefTable->fldId);

    if (isAggregate)
        select.join(p->mVesselLoglikeCatchesTable->name(), p->mVesselLoglikeTable->fldRowId, p->mVesselLoglikeCatchesTable->fldLoglikeId);

    select.where(op::eq(p->mVesselDefTable->fldNationality));
    select.groupBy(p->mVesselLoglikeTable->fldTStep);

    //qDebug() << "NationStat: " << QString::fromStdString(select.string()) << " : " << QString::fromStdString(nation);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    stmt.bind(std::make_tuple(nation));
    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });


    return data;
}

TimelineData SQLiteOutputStorage::getVesselLoglikeDataByHarbour(HarboursStat stattype, int harbourid, Operation op)
{
    bool isAggregate = false;
    if (stattype == HarboursStat::H_Catches || stattype == HarboursStat::H_Discards) {
        isAggregate = true;
    }

    FieldDef<FieldType::Real> f("");
    switch (stattype) {
    case HarboursStat::H_Catches:
        f = p->mVesselLoglikeCatchesTable->fldCatches;
        break;
    case HarboursStat::H_Discards:
        f = p->mVesselLoglikeCatchesTable->fldDiscards;
        break;
    case HarboursStat::H_Earnings:
        f = p->mVesselLoglikeTable->fldRevenueAV;
        break;
    case HarboursStat::H_Gav:
        f = p->mVesselLoglikeTable->gav;
        break;
    case HarboursStat::H_Vpuf:
        f = p->mVesselLoglikeTable->vpuf;
        break;
    case HarboursStat::H_SweptArea:
        f = p->mVesselLoglikeTable->sweptArea;
        break;
    case HarboursStat::H_RevenuePerSweptArea:
        f = p->mVesselLoglikeTable->revenuePerSweptArea;
        break;
    case HarboursStat::H_GVA:
        f = p->mVesselLoglikeTable->GVA;
        break;
    case HarboursStat::H_GVAPerRevenue:
        f = p->mVesselLoglikeTable->GVAPerRevenue;
        break;
    case HarboursStat::H_LabourSurplus:
        f = p->mVesselLoglikeTable->LabourSurplus;
        break;
    case HarboursStat::H_GrossProfit:
        f = p->mVesselLoglikeTable->GrossProfit;
        break;
    case HarboursStat::H_NetProfit:
        f = p->mVesselLoglikeTable->NetProfit;
        break;
    case HarboursStat::H_NetProfitMargin:
        f = p->mVesselLoglikeTable->NetProfitMargin;
        break;
    case HarboursStat::H_GVAPerFTE:
        f = p->mVesselLoglikeTable->GVAPerFTE;
        break;
    case HarboursStat::H_RoFTA:
        f = p->mVesselLoglikeTable->RoFTA;
        break;
    case HarboursStat::H_BER:
        f = p->mVesselLoglikeTable->BER;
        break;
    case HarboursStat::H_CRBER:
        f = p->mVesselLoglikeTable->CRBER;
        break;
    case HarboursStat::H_NetPresentValue:
        f = p->mVesselLoglikeTable->NetPresentValue;
        break;
    case HarboursStat::H_numTrips:
        f = sqlite::FieldDef<sqlite::FieldType::Real>("rowId");
        break;
    default:
        throw std::runtime_error("getVesselLoglikeDataByHarbour case not handled.");
    }

    switch (op) {
    case Operation::Sum:
        f = sqlite::op::sum(f);
        break;
    case Operation::Average:
        f = sqlite::op::avg(f);
        break;
    case Operation::Count:
        f = sqlite::op::count(f);
        break;
    }

    auto select = sqlite::statements::Select(p->mVesselLoglikeTable->name(),
                                                    p->mVesselLoglikeTable->fldTStep,
                                                    f
                                                    );
    select.join(p->mVesselDefTable->name(), p->mVesselLoglikeTable->fldId, p->mVesselDefTable->fldId);

    if (isAggregate)
        select.join(p->mVesselLoglikeCatchesTable->name(), p->mVesselLoglikeTable->fldRowId, p->mVesselLoglikeCatchesTable->fldLoglikeId);

    select.where(op::eq(p->mVesselLoglikeTable->fldLastHarbour));
    select.groupBy(p->mVesselLoglikeTable->fldTStep);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    stmt.bind(std::make_tuple(harbourid));
    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "HarbourStat: " << QString::fromStdString(select.string()) << " : " << harbourid << " => "  << data.v.size();

    return data;
}

TimelineData SQLiteOutputStorage::getVesselLoglikeDataByMetier(MetiersStat stattype, int metierid, Operation op)
{
    bool isAggregate = false;
    if (stattype == MetiersStat::M_Catches || stattype == MetiersStat::M_Discards) {
        isAggregate = true;
    }

    FieldDef<FieldType::Real> f("");
    switch (stattype) {
    case MetiersStat::M_Catches:
        f = p->mVesselLoglikeCatchesTable->fldCatches;
        break;
    case MetiersStat::M_Discards:
        f = p->mVesselLoglikeCatchesTable->fldDiscards;
        break;
    case MetiersStat::M_Revenues:
        f = p->mVesselLoglikeTable->fldRevenueAV;
        break;
    case MetiersStat::M_Gav:
        f = p->mVesselLoglikeTable->gav;
        break;
    case MetiersStat::M_Vpuf:
        f = p->mVesselLoglikeTable->vpuf;
        break;
    case MetiersStat::M_SweptArea:
        f = p->mVesselLoglikeTable->sweptArea;
        break;
    case MetiersStat::M_RevenuesPerSweptArea:
        f = p->mVesselLoglikeTable->revenuePerSweptArea;
        break;
    case MetiersStat::M_GVA:
        f = p->mVesselLoglikeTable->GVA;
        break;
    case MetiersStat::M_GVAPerRevenue:
        f = p->mVesselLoglikeTable->GVAPerRevenue;
        break;
    case MetiersStat::M_LabourSurplus:
        f = p->mVesselLoglikeTable->LabourSurplus;
        break;
    case MetiersStat::M_GrossProfit:
        f = p->mVesselLoglikeTable->GrossProfit;
        break;
    case MetiersStat::M_NetProfit:
        f = p->mVesselLoglikeTable->NetProfit;
        break;
    case MetiersStat::M_NetProfitMargin:
        f = p->mVesselLoglikeTable->NetProfitMargin;
        break;
    case MetiersStat::M_GVAPerFTE:
        f = p->mVesselLoglikeTable->GVAPerFTE;
        break;
    case MetiersStat::M_RoFTA:
        f = p->mVesselLoglikeTable->RoFTA;
        break;
    case MetiersStat::M_BER:
        f = p->mVesselLoglikeTable->BER;
        break;
    case MetiersStat::M_CRBER:
        f = p->mVesselLoglikeTable->CRBER;
        break;
    case MetiersStat::M_NetPresentValue:
        f = p->mVesselLoglikeTable->NetPresentValue;
        break;
    case MetiersStat::M_numTrips:
        f = sqlite::FieldDef<sqlite::FieldType::Real>("rowId");
        break;
    default:
        throw std::runtime_error("getVesselLoglikeDataByHarbour case not handled.");
    }

    switch (op) {
    case Operation::Sum:
        f = sqlite::op::sum(f);
        break;
    case Operation::Average:
        f = sqlite::op::avg(f);
        break;
    case Operation::Count:
        f = sqlite::op::count(f);
        break;
    }

    auto select = sqlite::statements::Select(p->mVesselLoglikeTable->name(),
                                                    p->mVesselLoglikeTable->fldTStep,
                                                    f
                                                    );
    select.join(p->mVesselDefTable->name(), p->mVesselLoglikeTable->fldId, p->mVesselDefTable->fldId);

    if (isAggregate)
        select.join(p->mVesselLoglikeCatchesTable->name(), p->mVesselLoglikeTable->fldRowId, p->mVesselLoglikeCatchesTable->fldLoglikeId);

    select.where(op::eq(p->mVesselLoglikeTable->fldMetierId));
    select.groupBy(p->mVesselLoglikeTable->fldTStep);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    stmt.bind(std::make_tuple(metierid));
    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "MetierStat: " << QString::fromStdString(select.string()) << " : " << metierid << " => "  << data.v.size();

    return data;
}

TimelineData SQLiteOutputStorage::getPopulationStatData(PopulationStat stat, AggregationType aggtype, int popid,
                                                        vector<int> szid)
{
    bool filterGrpId = (popid >= 0 && popid != 999);

    FieldDef<FieldType::Real> f("");
    FieldDef<FieldType::Real> fld("");
    switch (stat) {
    case displace::plot::PopulationStat::Aggregate:
        fld = p->mPopDynTable->fldN;
        break;
    case displace::plot::PopulationStat::Mortality:
        fld = p->mPopDynTable->fldF;
        break;
    case displace::plot::PopulationStat::SSB:
        fld = p->mPopDynTable->fldSSB;
        break;
    }

    switch (aggtype) {
    case displace::plot::AggregationType::Avg:
        f = op::avg(fld); break;
    case displace::plot::AggregationType::Min:
        f = op::min(fld); break;
    case displace::plot::AggregationType::Max:
        f = op::max(fld); break;
    case displace::plot::AggregationType::Sum:
        f = op::sum(fld); break;
    case displace::plot::AggregationType::None:
        f = fld;
        break;
    }

    auto select = sqlite::statements::Select(p->mPopDynTable->name(),
                                                    p->mPopDynTable->fldTStep,
                                                    f
                                                    );

    if (szid.size() > 0) {
        std::ostringstream ss;
        if (filterGrpId)
            ss << p->mPopDynTable->fldPopId.name() << " == ? AND ";
        ss << p->mPopDynTable->fldGroup.name() << " IN (?";
        for (size_t i = 1; i < szid.size() ; ++i)
            ss << ",?";
        ss << ")";
        select.where(ss.str());
    } else {
        if (filterGrpId)
            select.where(op::eq(p->mPopDynTable->fldPopId));
    }
/*
    if (szid >= 0) {
        select.where(op::and_(op::and_(op::eq(p->mPopDynTable->fldPopId), op::eq(p->mPopDynTable->fldGroup)), op::ne(fld)));
    } else {
        select.where(op::and_(op::eq(p->mPopDynTable->fldPopId), op::ne(fld)));
    }
*/
    select.groupBy(p->mPopDynTable->fldTStep);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    int n = 1;
    if (filterGrpId)
        stmt.bind(n++, popid);
    for (size_t i = 0; i < szid.size(); ++i)
        stmt.bind(n+i, szid[i]);

    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "PopDyn Select: " << QString::fromStdString(select.string()) << " (" << popid << "," << grpid << ")";

    return data;
}

TimelineData SQLiteOutputStorage::getBenthosStatData(BenthosStat stat, AggregationType aggtype, int grpid, const std::vector<int> &btype)
{
    bool filterGrpId = (grpid >= 0 && grpid != 999);

    FieldDef<FieldType::Real> f("");
    FieldDef<FieldType::Real> fld("");
    switch (stat) {
    case displace::plot::BenthosStat::B_MeanWeight:
        fld = p->mFuncGroupsTable->benthosMeanW;
        break;
    case displace::plot::BenthosStat::B_Number:
        fld = p->mFuncGroupsTable->benthosNumTot;
        break;
    case displace::plot::BenthosStat::B_NumberOverK:
        fld = p->mFuncGroupsTable->benthosNumK;
        break;
    case displace::plot::BenthosStat::B_TotBiomass:
        fld = p->mFuncGroupsTable->benthosBio;
        break;
    case displace::plot::BenthosStat::B_TotBiomassOverK:
        fld = p->mFuncGroupsTable->benthosBioK;
        break;
    }

    switch (aggtype) {
    case displace::plot::AggregationType::Avg:
        f = op::avg(fld); break;
    case displace::plot::AggregationType::Min:
        f = op::min(fld); break;
    case displace::plot::AggregationType::Max:
        f = op::max(fld); break;
    case displace::plot::AggregationType::Sum:
        f = op::sum(fld); break;
    case displace::plot::AggregationType::None:
        f = fld;
        break;
    }

    auto select = sqlite::statements::Select(p->mFuncGroupsTable->name(),
                                                    p->mFuncGroupsTable->fldTStep,
                                                    f
                                                    );

    if (btype.size() > 0) {
        std::ostringstream ss;
        if (filterGrpId)
            ss << p->mFuncGroupsTable->fldFGroup.name() << " == ? AND ";
        ss << p->mFuncGroupsTable->fldBType.name() << " IN (?";
        for (size_t i = 1; i < btype.size() ; ++i)
            ss << ",?";
        ss << ")";
        select.where(ss.str());
    } else {
        if (filterGrpId)
            select.where(op::eq(p->mFuncGroupsTable->fldFGroup));
    }

    select.groupBy(p->mFuncGroupsTable->fldTStep);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    int n = 1;
    if (filterGrpId)
        stmt.bind(n++, grpid);
    for (size_t i = 0; i < btype.size(); ++i)
        stmt.bind(n+i, btype[i]);

    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "FunGroup Select: " << QString::fromStdString(select.string()) << " (" << grpid << "," << btype << "): " << data.t.size();

    return data;
}

TimelineData SQLiteOutputStorage::getFishFarmStatData(displace::plot::FishfarmsStat stattype,
                                                      displace::plot::AggregationType aggtype, int ffid,
                                                      std::vector<int> ftypes)
{
    bool filterGrpId = (ffid >= 0 && ffid != 999);

    FieldDef<FieldType::Real> f("");
    FieldDef<FieldType::Real> fld("");
    switch (stattype) {
        case displace::plot::FishfarmsStat::FF_FishHarvestedKg:
            fld = p->mFishfarmsTable->fldFish;
            break;
        case displace::plot::FishfarmsStat::FF_EggsHarvestedKg:
            fld = p->mFishfarmsTable->fldEggs;
            break;
        case displace::plot::FishfarmsStat::FF_AnnualProfit:
            fld = p->mFishfarmsTable->fldProfit;
            break;
        case displace::plot::FishfarmsStat::FF_FishMeanWeight:
        case displace::plot::FishfarmsStat::FF_NetDischargeN:
        case displace::plot::FishfarmsStat::FF_NetDischargeP:
        case displace::plot::FishfarmsStat::FF_CumulNetDischargeN:
        case displace::plot::FishfarmsStat::FF_CumulNetDischargeP:
            throw std::logic_error("Unhandled case - Unimplemented");
    }

    switch (aggtype) {
        case displace::plot::AggregationType::Avg:
            f = op::avg(fld); break;
        case displace::plot::AggregationType::Min:
            f = op::min(fld); break;
        case displace::plot::AggregationType::Max:
            f = op::max(fld); break;
        case displace::plot::AggregationType::Sum:
            f = op::sum(fld); break;
        case displace::plot::AggregationType::None:
            f = fld;
            break;
    }

    auto select = sqlite::statements::Select(p->mFishfarmsTable->name(),
                                             p->mFishfarmsTable->fldTStep,
                                             f
    );

    if (ftypes.size() > 0) {
        std::ostringstream ss;
        if (filterGrpId)
            ss << p->mFishfarmsTable->fldFarmId.name() << " == ? AND ";
        ss << p->mFishfarmsTable->fldFarmType.name() << " IN (?";
        for (size_t i = 1; i < ftypes.size() ; ++i)
            ss << ",?";
        ss << ")";
        select.where(ss.str());
    } else {
        if (filterGrpId)
            select.where(op::eq(p->mFishfarmsTable->fldFarmId));
    }

    select.groupBy(p->mFishfarmsTable->fldTStep);

    sqlite::SQLiteStatement stmt(p->db,select);

    TimelineData data;

    int n = 1;
    if (filterGrpId)
        stmt.bind(n++, ffid);
    for (size_t i = 0; i < ftypes.size(); ++i)
        stmt.bind(n+i, ftypes[i]);

    stmt.execute([&stmt, &data](){
        data.t.push_back(stmt.getIntValue(0));
        data.v.push_back(stmt.getDoubleValue(1));
        return true;
    });

    //qDebug() << "FunGroup Select: " << QString::fromStdString(select.string()) << " (" << grpid << "," << btype << "): " << data.t.size();

    return data;
}

TimelineData SQLiteOutputStorage::getShipsStatData(ShipsStat stattype, AggregationType aggtype, int shipid, std::vector<int> shiptypeid)
{
    return p->mShipsTable->getShipsStatData (stattype,aggtype, shipid, shiptypeid);
}

size_t SQLiteOutputStorage::getNbPops()
{
    return p->mPopTable->getNbPops();
}

std::vector<std::string> SQLiteOutputStorage::getNationsList()
{
    return p->mVesselDefTable->getNationsList();
}

void SQLiteOutputStorage::createAllTables()
{
    p->mVesselDefTable->dropAndCreate();
    p->mVesselLoglikeTable->dropAndCreate();
    p->mVesselVmslikeTable->dropAndCreate();
    p->mVesselVmslikeFPingsOnlyTable->dropAndCreate();
    p->mNodesDefTable->dropAndCreate(); 
    p->mNodesEnvtTable->dropAndCreate();
    p->mNodesStatTable->dropAndCreate();
    p->mNodesTariffStatTable->dropAndCreate();
    p->mShipsTable->dropAndCreate();
    p->mPopDynTable->dropAndCreate();
    p->mPopTable->dropAndCreate();
    p->mFuncGroupsTable->dropAndCreate();
    p->mFishfarmsTable->dropAndCreate();
    p->mWindmillsTable->dropAndCreate();
    p->mVesselLoglikeCatchesTable->dropAndCreate();

    p->mMetadata->dropAndCreate();
    std::ostringstream ss;
    ss << CURRENT_DB_SCHEMA_VERSION;
    p->mMetadata->setMetadata("dbVersion", ss.str());
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

std::shared_ptr<VesselVmsLikeFPingsOnlyTable> SQLiteOutputStorage::getVesselVmsLikeFPingsOnlyTable() const
{
    return p->mVesselVmslikeFPingsOnlyTable;
}

std::shared_ptr<NodesDefTable> SQLiteOutputStorage::getNodesDefTable() const
{
    return p->mNodesDefTable;
}

std::shared_ptr<NodesEnvtTable> SQLiteOutputStorage::getNodesEnvtTable() const
{
    return p->mNodesEnvtTable;
}

std::shared_ptr<NodesStatTable> SQLiteOutputStorage::getNodesStatTable() const
{
    return p->mNodesStatTable;
}

std::shared_ptr<NodesTariffStatTable> SQLiteOutputStorage::getNodesTariffStatTable() const
{
    return p->mNodesTariffStatTable;
}

std::shared_ptr<PopTable> SQLiteOutputStorage::getPopTable() const
{
    return p->mPopTable;
}

std::shared_ptr<FuncGroupsTable> SQLiteOutputStorage::getFuncGroupsTable() const
{
    return p->mFuncGroupsTable;
}

std::shared_ptr<WindfarmsTable> SQLiteOutputStorage::getWindfarmTable() const
{
    return p->mWindmillsTable;
}

std::shared_ptr<MetadataTable> SQLiteOutputStorage::metadata() const
{
    return p->mMetadata;
}
