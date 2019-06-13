#include "vesselvmsliketable.h"

#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/sqlitestatement.h"
#include "msqlitecpp/v1/sqlitefieldsop.h"
#include "msqlitecpp/v1/insertstatement.h"
#include "msqlitecpp/v1/createstatement.h"

using namespace sqlite;

struct VesselVmsLikeTable::Impl {
    std::mutex mutex;
    bool init = false;

    using ThisInsertStatement = InsertStatement<
            decltype(VesselVmsLikeTable::fldId),
            decltype(VesselVmsLikeTable::fldTStep),
            decltype(VesselVmsLikeTable::fldTStepDep),
            decltype(VesselVmsLikeTable::fldPosLong),
            decltype(VesselVmsLikeTable::fldPosLat),
            decltype(VesselVmsLikeTable::fldCourse),
            decltype(VesselVmsLikeTable::fldCumFuel),
            decltype(VesselVmsLikeTable::fldState)>;

    std::shared_ptr<ThisInsertStatement> statement;

    sqlite::SQLiteStatement vesselTStepSelect;
};

VesselVmsLikeTable::VesselVmsLikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
        : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselVmsLikeTable::~VesselVmsLikeTable() noexcept = default;

void VesselVmsLikeTable::dropAndCreate()
{
    if (db()->tableExists(name())) {
        db()->dropTable(name());
    }

    auto def = std::make_tuple(
            fldId, fldTStep, fldTStepDep,
            fldPosLong, fldPosLat, fldCourse,
            fldCumFuel,
            fldState
    );

    create(def);

    auto index = sqlite::makeCreateUniqueIndexStatement(db(), "index" + name(), name(), fldId, fldTStep);
    index.unique();
    index.execute();
}

void VesselVmsLikeTable::init()
{
    if (!p->init) {
        p->init = true;

        p->statement = std::make_shared<Impl::ThisInsertStatement>(fldId, fldTStep, fldTStepDep,
                                                                   fldPosLong, fldPosLat, fldCourse,
                                                                   fldCumFuel,
                                                                   fldState);
        p->statement->replaceOnConflict();
        p->statement->attach(db(), name());
        p->statement->prepare();

        auto select = sqlite::statements::Select(name(),
                                                 fldId, fldTStep, fldTStepDep,
                                                 fldPosLong, fldPosLat, fldCourse,
                                                 fldCumFuel,
                                                 fldState,
                                                 op::max(fldTStep))
                .where(op::le(fldTStep))
                .groupBy(fldId);
        p->vesselTStepSelect = std::move(sqlite::SQLiteStatement(db(), select));
    }
}

void VesselVmsLikeTable::insertLog(const VesselVmsLikeTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);
    init();

    p->statement->insert(log.id,
                         log.tstep,
                         log.tstep_dep,
                         log.p_long,
                         log.p_lat,
                         log.p_course,
                         log.cum_fuel,
                         log.state);
}

void VesselVmsLikeTable::queryAllVesselsAtStep(int tstep, std::function<bool(const VesselVmsLikeTable::Log &)> op)
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

        if (op) {
            return op(l);
        }
        return false;
    });
}
