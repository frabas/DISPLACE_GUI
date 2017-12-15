#include "vesselvmsliketable.h"

VesselVmsLikeTable::VesselVmsLikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
    if (db->tableExists(name))
        db->dropTable(name);

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
    SQLiteTable::insert(fldId.assign(log.id),
                        fldTStep.assign(log.tstep),
                        fldTStepDep.assign(log.tstep_dep),
                        fldPosLong.assign(log.p_long),
                        fldPosLat.assign(log.p_lat),
                        fldCourse.assign(log.p_course),
                        fldCumFuel.assign(log.cum_fuel),
                        fldState.assign(log.state)
                        );
}
