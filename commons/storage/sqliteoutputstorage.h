#ifndef SQLITEOUTPUTSTORAGE_H
#define SQLITEOUTPUTSTORAGE_H

#include "commons_global.h"

#include "plottypes.h"

#include <memory>
#include <string>
#include <vector>

class VesselDefTable;
class VesselsLoglikeTable;
class VesselVmsLikeTable;
class VesselVmsLikeFPingsOnlyTable;
class NodesDefTable;
class NodesStatTable;
class PopTable;
class Fishfarm;
class Windmill;
class WindfarmsTable;
class Vessel;
class Population;
class Node;
class MetadataTable;
class FuncGroupsTable;

namespace sqlite {
class SQLiteStorage;
}

struct TimelineData {
    std::vector<double> t;
    std::vector<double> v;
};

class COMMONSSHARED_EXPORT SQLiteOutputStorage
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    SQLiteOutputStorage(std::string path);
    ~SQLiteOutputStorage() noexcept;

    void open();
    void close();

    void startDayLoop();
    void endDayLoop();

    void createAllTables();
    void createAllIndexes();

    std::shared_ptr<VesselDefTable> getVesselDefTable() const;
    std::shared_ptr<VesselsLoglikeTable> getVesselLoglikeTable() const;
    std::shared_ptr<VesselVmsLikeTable> getVesselVmsLikeTable() const;
    std::shared_ptr<VesselVmsLikeFPingsOnlyTable> getVesselVmsLikeFPingsOnlyTable() const;
    std::shared_ptr<NodesDefTable> getNodesDefTable() const;
    std::shared_ptr<NodesStatTable> getNodesStatTable() const;
    std::shared_ptr<PopTable> getPopTable() const;
    std::shared_ptr<FuncGroupsTable> getFuncGroupsTable() const;
    std::shared_ptr<WindfarmsTable> getWindfarmTable() const;
    std::shared_ptr<MetadataTable> metadata() const;

    std::shared_ptr<sqlite::SQLiteStorage> getDb() const;

    void exportFishfarmLog(Fishfarm *fishfarm, int tstep);
    void exportWindmillsLog(Windmill *windmill, int tstep);
    void exportPopNodes(int tstep, Node *node);
    void exportPopStat(Population *pop, int popid, int tstep);
    void exportLogLike(Vessel *v, const std::vector<double> &populations, const std::vector<double> &discards, unsigned int tstep);
    void exportCalendar (const std::vector<int> &tsteps_months, const std::vector<int> &tsteps_quarters,
                         const std::vector<int> &tsteps_semesters, const std::vector<int> &tsteps_years);
    void importCalendar (std::vector<int> &tsteps_months, std::vector<int> &tsteps_quarters,
                         std::vector<int> &tsteps_semesters, std::vector<int> &tsteps_years);

    enum Operation { Sum, Average, Count };
    TimelineData getVesselLoglikeDataByNation(displace::plot::NationsStat stattype, std::string nation, Operation op);
    TimelineData getVesselLoglikeDataByHarbour(displace::plot::HarboursStat stattype, int harbourid, Operation op);
    TimelineData getVesselLoglikeDataByMetier(displace::plot::MetiersStat stattype, int metierid, Operation op);
    TimelineData getPopulationStatData(displace::plot::PopulationStat stat, displace::plot::AggregationType aggtype, int popid, int grpid);
    TimelineData getBenthosStatData(displace::plot::BenthosStat stattype, displace::plot::AggregationType aggtype, int grpid, const std::vector<int> &btype);

    size_t getNbPops();

    std::vector<std::string> getNationsList();
private:
};

#endif // SQLITEOUTPUTSTORAGE_H
