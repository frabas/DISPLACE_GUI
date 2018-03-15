#include "vesselvmslikefpingsonlytable.h"

#include <sqlitestatementformatters.h>
#include <sqlitestatement.h>
#include <sqlitefieldsop.h>

struct VesselVmsLikeFPingsOnlyTable::Impl
{
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,
                    FieldDef<FieldType::Integer> ,
                    FieldDef<FieldType::Integer> ,
                    //FieldDef<FieldType::Real>,
                    //FieldDef<FieldType::Real>,
                    //FieldDef<FieldType::Real>,
                    //FieldDef<FieldType::Real>,
                    FieldDef<FieldType::Integer>,
                    FieldDef<FieldType::Integer>
    > statement;

    sqlite::SQLiteStatement vesselTStepSelect;
};

VesselVmsLikeFPingsOnlyTable::VesselVmsLikeFPingsOnlyTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselVmsLikeFPingsOnlyTable::~VesselVmsLikeFPingsOnlyTable() noexcept = default;

void VesselVmsLikeFPingsOnlyTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    auto def = std::make_tuple (
                fldId, fldTStep, fldTStepDep,
                //fldPosLong, fldPosLat, fldCourse,
                //fldCumFuel,
                fldNodeId,
                fldPopId
                );

    create(def);
}

void VesselVmsLikeFPingsOnlyTable::init()
{
    if (!p->init) {
        p->init = true;

        p->statement = prepareInsert(std::make_tuple(fldId, fldTStep, fldTStepDep,
                                     //fldPosLong, fldPosLat, fldCourse,
                                     //fldCumFuel,
                                     fldNodeId,
                                     fldPopId));


        auto select = sqlite::statements::Select(name(),
                                                 fldId, fldTStep, fldTStepDep,
                                                 //fldPosLong, fldPosLat, fldCourse,
                                                 //fldCumFuel,
                                                 fldNodeId,
                                                 fldPopId,
                                                 op::max(fldTStep))
                .where(op::le(fldTStep))
                .groupBy(fldId);
        p->vesselTStepSelect = std::move(sqlite::SQLiteStatement(db(), select));
    }
}

void VesselVmsLikeFPingsOnlyTable::insertLog(const VesselVmsLikeFPingsOnlyTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);
    init();

    SQLiteTable::insert(p->statement, std::make_tuple(log.id,
                        log.tstep,
                        log.tstep_dep,
                        //log.p_long,
                        //log.p_lat,
                        //log.p_course,
                        //log.cum_fuel,
                        log.nodeid,
                        log.popid)
                        );
}

void VesselVmsLikeFPingsOnlyTable::queryAllVesselsAtStep(int tstep, std::function<bool (const VesselVmsLikeFPingsOnlyTable::Log &)> op)
{
    std::unique_lock<std::mutex> m(p->mutex);
    init();

    p->vesselTStepSelect.bind(1, tstep);

    p->vesselTStepSelect.execute([this, &op] {
        auto &stmt = p->vesselTStepSelect;
        Log l;
        l.id = stmt.getIntValue(0);
        l.tstep = stmt.getIntValue(1);
        //l.p_long = stmt.getDoubleValue(3);
        //l.p_lat = stmt.getDoubleValue(4);
        //l.p_course = stmt.getDoubleValue(5);
        //l.cum_fuel = stmt.getDoubleValue(6);
        l.nodeid = stmt.getIntValue(7);
        l.popid = stmt.getIntValue(8);

        if (op)
            return op(l);
        return false;
    });
}
