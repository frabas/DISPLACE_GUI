#ifndef VESSELVMSLIKETABLE_H
#define VESSELVMSLIKETABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"

#include <vector>
#include <string>

using namespace sqlite;

class COMMONSSHARED_EXPORT VesselVmsLikeTable : public sqlite::SQLiteTable
{
    const FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldTStepDep = makeFieldDef("TStepDep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Real> fldPosLong = makeFieldDef("Long", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> fldPosLat = makeFieldDef("Lat", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> fldCourse = makeFieldDef("Course", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> fldCumFuel = makeFieldDef("CumFuel", FieldType::Real()).notNull();
    const FieldDef<FieldType::Integer> fldState = makeFieldDef("State", FieldType::Integer()).notNull();

public:
    VesselVmsLikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    void dropAndCreate();

    struct Log {
        int id;
        int tstep, tstep_dep;
        double p_long, p_lat, p_course, cum_fuel;
        int state;
    };

    void insertLog (const Log &log);
};

#endif // VESSELVMSLIKETABLE_H
