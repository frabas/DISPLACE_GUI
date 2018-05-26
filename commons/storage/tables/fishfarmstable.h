#ifndef FISHFARMSTABLE_H
#define FISHFARMSTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Fishfarm;

class COMMONSSHARED_EXPORT FishfarmsTable : public SQLiteTable
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldFarmId = makeFieldDef("FarmId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldFarmType = makeFieldDef("FarmType",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> fldMeanW = makeFieldDef("MeanW", FieldType::Real());
    FieldDef<FieldType::Real> fldFish = makeFieldDef("Fish", FieldType::Real());
    FieldDef<FieldType::Real> fldEggs = makeFieldDef("Eggs", FieldType::Real());
    FieldDef<FieldType::Real> fldProfit = makeFieldDef("Profit", FieldType::Real());

    FishfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    ~FishfarmsTable() noexcept;
    void dropAndCreate();

    void exportFishfarmLog(Fishfarm *fishfarm, int tstep);
};

#endif // FISHFARMSTABLE_H
