#ifndef SHIPSTABLE_H
#define SHIPSTABLE_H

#include "commons_global.h"
#include "idtypes.h"
#include "dbtypes.h"
#include "plottypes.h"

#include "msqlitecpp/v1/sqlitetable.h"

#include <string>

class Ship;

class  ShipsTable : public sqlite::SQLiteTable {
    struct Impl;
    std::unique_ptr<Impl> p;

    void init();

public:
    ShipsTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~ShipsTable() noexcept;

    void dropAndCreate();

    void exportShipsIndivators(int tstep, Ship *ship);

    TimelineData
    getShipsStatData(displace::plot::ShipsStat stattype, displace::plot::AggregationType aggtype, int shipid,
                     std::vector<int> shiptypeid);
};

#endif // SHIPSTABLE_H
