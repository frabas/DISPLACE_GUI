#include "vesselvmsliketable.h"

struct VesselVmsLikeTable::Impl
{
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,
                    FieldDef<FieldType::Integer> ,
                    FieldDef<FieldType::Integer> ,
                    FieldDef<FieldType::Real>,
                    FieldDef<FieldType::Real>,
                    FieldDef<FieldType::Real>,
                    FieldDef<FieldType::Real>,
                    FieldDef<FieldType::Integer>
    > statement;
};

VesselVmsLikeTable::VesselVmsLikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselVmsLikeTable::~VesselVmsLikeTable() noexcept = default;

void VesselVmsLikeTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    auto def = std::make_tuple (
                fldId, fldTStep, fldTStepDep,
                fldPosLong, fldPosLat, fldCourse,
                fldCumFuel,
                fldState
                );

    create(def);
}


void VesselVmsLikeTable::insertLog(const VesselVmsLikeTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;

        p->statement = prepareInsert(std::make_tuple(fldId, fldTStep, fldTStepDep,
                                     fldPosLong, fldPosLat, fldCourse,
                                     fldCumFuel,
                                     fldState));
    }

    SQLiteTable::insert(p->statement, std::make_tuple(log.id,
                        log.tstep,
                        log.tstep_dep,
                        log.p_long,
                        log.p_lat,
                        log.p_course,
                        log.cum_fuel,
                        log.state)
                        );
}
