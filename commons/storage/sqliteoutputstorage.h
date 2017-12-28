#ifndef SQLITEOUTPUTSTORAGE_H
#define SQLITEOUTPUTSTORAGE_H

#include "commons_global.h"

#include <memory>
#include <string>

class VesselDefTable;
class VesselsLoglikeTable;
class VesselVmsLikeTable;
class PopNodesTable;
class PopTable;
class Fishfarm;
class Windmill;

namespace sqlite {
class SQLiteStorage;
}

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
    std::shared_ptr<VesselDefTable> getVesselDefTable() const;
    std::shared_ptr<VesselsLoglikeTable> getVesselLoglikeTable() const;
    std::shared_ptr<VesselVmsLikeTable> getVesselVmsLikeTable() const;
    std::shared_ptr<PopNodesTable> getPopNodesTable() const;
    std::shared_ptr<PopTable> getPopTable() const;

    std::shared_ptr<sqlite::SQLiteStorage> getDb() const;

    void exportFishfarmLog(Fishfarm *fishfarm, int tstep);
    void exportWindmillsLog(Windmill *windmill, int tstep);
};

#endif // SQLITEOUTPUTSTORAGE_H
