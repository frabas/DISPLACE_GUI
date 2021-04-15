#ifndef SQLITEOUTPUTSTORAGE_H
#define SQLITEOUTPUTSTORAGE_H

#include "commons_global.h"
#include "dbtypes.h"

#include "plottypes.h"

#include <memory>
#include <string>
#include <vector>

class VesselDefTable;
class VesselsLoglikeTable;
class VesselVmsLikeTable;
class VesselVmsLikeFPingsOnlyTable;
class NodesDefTable;
class NodesEnvtTable;
class NodesStatTable;
class NodesTariffStatTable;
class PopTable;
class Fishfarm;
class Windmill;
class WindfarmsTable;
class Vessel;
class Ship;
class Population;
class Node;
class MetadataTable;
class FuncGroupsTable;
class FishFarmsDefTable;

namespace sqlite {
class SQLiteStorage;
}

class  SQLiteOutputStorage
{
    struct Impl;
    std::unique_ptr<Impl> p;

    static const int CURRENT_DB_SCHEMA_VERSION;
public:
    explicit SQLiteOutputStorage(std::string path);
    ~SQLiteOutputStorage() noexcept;

    void open();
    void close();

    ///< Get the current db version number from the metadata table
    int versionNumber();

    ///< Get the current supported db version number schema
    int currentSchemaNumber();

    bool isOutdated() { return versionNumber() < currentSchemaNumber(); }

    void startDayLoop();
    void endDayLoop();

    void createAllTables();
    void createAllIndexes();

    std::shared_ptr<VesselDefTable> getVesselDefTable() const;
    std::shared_ptr<VesselsLoglikeTable> getVesselLoglikeTable() const;
    std::shared_ptr<VesselVmsLikeTable> getVesselVmsLikeTable() const;
    std::shared_ptr<VesselVmsLikeFPingsOnlyTable> getVesselVmsLikeFPingsOnlyTable() const;
    std::shared_ptr<NodesDefTable> getNodesDefTable() const;
    std::shared_ptr<NodesEnvtTable> getNodesEnvtTable() const;
    std::shared_ptr<NodesStatTable> getNodesStatTable() const;
    std::shared_ptr<NodesTariffStatTable> getNodesTariffStatTable() const;
    std::shared_ptr<PopTable> getPopTable() const;
    std::shared_ptr<FuncGroupsTable> getFuncGroupsTable() const;
    std::shared_ptr<WindfarmsTable> getWindfarmTable() const;
    std::shared_ptr<FishFarmsDefTable> getFishfarmDefTable() const;
    std::shared_ptr<MetadataTable> metadata() const;

    std::shared_ptr<sqlite::SQLiteStorage> getDb() const;

    void exportFishfarmDef(const Fishfarm &fishfarm);
    void exportFishfarmLog(Fishfarm *fishfarm, int tstep);
    void exportWindmillsLog(Windmill *windmill, int tstep);
    void exportPopNodes(int tstep, Node *node);
    void exportEnvtNodes(int tstep, Node *node);
    void exportTariffNodes(int tstep, Node *node);
    void exportPopStat(Population *pop, int popid, int tstep);
    void exportPopQuotas(Population *pop, int popid, int tstep);
    void exportLogLike(Vessel *v, const std::vector<double> &populations, const std::vector<double> &discards, unsigned int tstep,
                      unsigned int export_discards_in_logbooks);
    void exportCalendar (const std::vector<int> &tsteps_months, const std::vector<int> &tsteps_quarters,
                         const std::vector<int> &tsteps_semesters, const std::vector<int> &tsteps_years);
    void exportShip (int tstep, Ship *ship);
    void importCalendar (std::vector<int> &tsteps_months, std::vector<int> &tsteps_quarters,
                         std::vector<int> &tsteps_semesters, std::vector<int> &tsteps_years);

    enum Operation { Sum, Average, Count };
    TimelineData getVesselLoglikeDataByNation(displace::plot::NationsStat stattype, std::string nation, Operation op);
    TimelineData getVesselLoglikeDataByVessel(displace::plot::VesselsStat stattype, std::string vessel, Operation op);
    TimelineData getVesselLoglikeDataByHarbour(displace::plot::HarboursStat stattype, int harbourid, Operation op);
    TimelineData getVesselLoglikeDataByMetier(displace::plot::MetiersStat stattype, int metierid, Operation op);
    TimelineData getPopulationStatData(displace::plot::PopulationStat stat, displace::plot::AggregationType aggtype,
                                       int popid, std::vector<int> szid);
    TimelineData getBenthosStatData(displace::plot::BenthosStat stattype, displace::plot::AggregationType aggtype, int grpid, const std::vector<int> &btype);
    TimelineData getShipsStatData(displace::plot::ShipsStat stattype, displace::plot::AggregationType aggtype, int shipid, std::vector<int> shiptypeid);
    TimelineData getFishFarmStatData(displace::plot::FishfarmsStat stattype, displace::plot::AggregationType aggtype, int ffid, std::vector<int> ftypes);

    size_t getNbPops();

    std::vector<std::string> getNationsList();
    std::vector<std::string> getPopulationsList();
private:
};

#endif // SQLITEOUTPUTSTORAGE_H
