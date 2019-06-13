#ifndef VESSELVMSLIKETABLE_H
#define VESSELVMSLIKETABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include <functional>

#include "msqlitecpp/v1/sqlitefielddef.h"
#include "msqlitecpp/v1/sqlitetable.h"

#include <vector>
#include <string>

using namespace sqlite;

class COMMONSSHARED_EXPORT VesselVmsLikeTable : public sqlite::SQLiteTable {
private:
    struct Impl;
    std::unique_ptr<Impl> p;

    void init();

public:
    FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldTStepDep = makeFieldDef("TStepDep", FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> fldPosLong = makeFieldDef("Long", FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldPosLat = makeFieldDef("Lat", FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldCourse = makeFieldDef("Course", FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldCumFuel = makeFieldDef("CumFuel", FieldType::Real()).notNull();
    FieldDef<FieldType::Integer> fldState = makeFieldDef("State", FieldType::Integer()).notNull();

public:
    VesselVmsLikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~VesselVmsLikeTable() noexcept;

    void dropAndCreate();

    struct Log {
        int id;
        int tstep, tstep_dep;
        double p_long, p_lat, p_course, cum_fuel;
        int state;
    };

    void insertLog(const Log &log);

    void queryAllVesselsAtStep(int tstep, std::function<bool(const Log &)>);
};

#endif // VESSELVMSLIKETABLE_H
