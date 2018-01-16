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
class PopNodesTable;
class PopTable;
class Fishfarm;
class Windmill;
class WindfarmsTable;
class Vessel;
class Population;

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
    std::shared_ptr<PopNodesTable> getPopNodesTable() const;
    std::shared_ptr<PopTable> getPopTable() const;
    std::shared_ptr<WindfarmsTable> getWindfarmTable() const;

    std::shared_ptr<sqlite::SQLiteStorage> getDb() const;

    void exportFishfarmLog(Fishfarm *fishfarm, int tstep);
    void exportWindmillsLog(Windmill *windmill, int tstep);
    void exportLogLike(Vessel *v, const std::vector<double> &populations, unsigned int tstep);

    TimelineData getVesselLoglikeDataByNation(displace::plot::NationsStat stattype, std::string nation);

private:
};

#endif // SQLITEOUTPUTSTORAGE_H
