#ifndef VESSELVMSLIKEFPINGSONLYTABLE_H
#define VESSELVMSLIKEFPINGSONLYTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"

#include <vector>
#include <string>

using namespace sqlite;

class COMMONSSHARED_EXPORT VesselVmsLikeFPingsOnlyTable : public sqlite::SQLiteTable
{
private:
    struct Impl;
    std::unique_ptr<Impl> p;

    void init();
public:
    const FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldTStepDep = makeFieldDef("TStepDep",FieldType::Integer()).notNull();
    //const FieldDef<FieldType::Real> fldPosLong = makeFieldDef("Long", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldPosLat = makeFieldDef("Lat", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldCourse = makeFieldDef("Course", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldCumFuel = makeFieldDef("CumFuel", FieldType::Real()).notNull();
    const FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId", FieldType::Integer()).notNull();

public:
    VesselVmsLikeFPingsOnlyTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~VesselVmsLikeFPingsOnlyTable() noexcept;

    void dropAndCreate();

    struct Log {
        int id;
        int tstep, tstep_dep;
        //double p_long, p_lat, p_course, cum_fuel;
        int nodeid, popid;
    };

    void insertLog (const Log &log);
    void queryAllVesselsAtStep (int tstep, std::function<bool(const Log &)>);
};

#endif // VESSELVMSLIKEFPINGSONLYTABLE_H
