#include "vesselvmsliketable.h"

#include <sqlitestatementformatters.h>
#include <sqlitestatement.h>
#include <sqlitefieldsop.h>

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

    sqlite::SQLiteStatement vesselTStepSelect;
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

void VesselVmsLikeTable::init()
{
    if (!p->init) {
        p->init = true;

        p->statement = prepareInsert(std::make_tuple(fldId, fldTStep, fldTStepDep,
                                     fldPosLong, fldPosLat, fldCourse,
                                     fldCumFuel,
                                     fldState));


        auto select = sqlite::statements::Select(name(),
                                                 fldId, fldTStep, fldTStepDep,
                                                 fldPosLong, fldPosLat, fldCourse,
                                                 fldCumFuel,
                                                 fldState).where(op::eq(fldTStep));

        p->vesselTStepSelect = std::move(sqlite::SQLiteStatement(db(), select));
    }
}

void VesselVmsLikeTable::insertLog(const VesselVmsLikeTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);
    init();

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

void VesselVmsLikeTable::queryAllVesselsAtStep(int tstep, std::function<bool (const VesselVmsLikeTable::Log &)> op)
{
    std::unique_lock<std::mutex> m(p->mutex);
    init();

    p->vesselTStepSelect.bind(1, tstep);

    p->vesselTStepSelect.execute([this, &op] {
        auto &stmt = p->vesselTStepSelect;
        Log l;
        l.id = stmt.getIntValue(0);
        l.tstep = stmt.getIntValue(1);
        l.p_long = stmt.getDoubleValue(3);
        l.p_lat = stmt.getDoubleValue(4);
        l.p_course = stmt.getDoubleValue(5);
        l.cum_fuel = stmt.getDoubleValue(6);
        l.state = stmt.getIntValue(7);

        if (op)
            return op(l);
        return false;
    });
}
