#include "windfarmstable.h"

#include <Windmill.h>
#include <sqlitestatement.h>
#include <sqlitefieldsop.h>
#include <sqlitestatementformatters.h>

struct WindfarmsTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>> statement;
};


WindfarmsTable::WindfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

WindfarmsTable::~WindfarmsTable() noexcept = default;

void WindfarmsTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldTStep,
                           fldWindfarmId,
                           fldWindfarmType,
                           fldKwh,
                           fldKwhProduction
                           ));
}

void WindfarmsTable::exportWindmillData(Windmill *windmill, int tstep)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldTStep,
                                                     fldWindfarmId,
                                                     fldWindfarmType,
                                                     fldKwh,fldKwhProduction));
    }

    insert(p->statement, std::make_tuple(tstep,
           windmill->get_idx(),
           windmill->get_type(),
           windmill->get_kWh(),
           windmill->get_kW_production())
           );
}

WindfarmsTable::StatData WindfarmsTable::getStatData(WindfarmsTable::StatType stattype, WindfarmsTable::Aggreg aggreg)
{
    return getStatData(stattype, aggreg, -1);
}

WindfarmsTable::StatData WindfarmsTable::getStatData(WindfarmsTable::StatType stattype, WindfarmsTable::Aggreg aggreg, int wfType)
{
    sqlite::FieldDef<sqlite::FieldType::Real> op = fldKwh;

    switch (stattype) {
    case WindfarmsTable::StatType::Kwh:
        op = fldKwh; break;
    case WindfarmsTable::StatType::KwhProduction:
        op = fldKwhProduction; break;
    }

    switch (aggreg) {
    case WindfarmsTable::Aggreg::Sum:
        op = sqlite::op::sum(op);
        break;
    case WindfarmsTable::Aggreg::Avg:
        op = sqlite::op::avg(op);
        break;
    case WindfarmsTable::Aggreg::Min:
        op = sqlite::op::min(op);
        break;
    case WindfarmsTable::Aggreg::Max:
        op = sqlite::op::max(op);
        break;
    }

    sqlite::statements::Select s(name(), fldTStep, op);
    s.groupBy(fldTStep);

    if (wfType != -1) {
        s.where(sqlite::op::eq(fldWindfarmType));
    }

    sqlite::SQLiteStatement smt (db(), s);

    if (wfType != -1) {
        smt.bind(std::make_tuple(wfType));
    }

    WindfarmsTable::StatData d;

    smt.execute([&d, &smt]() {
        auto tstep = smt.getIntValue(0);
        auto v = smt.getDoubleValue(1);
        d.t.push_back(tstep);
        d.v.push_back(v);
        return true;
    });

    return d;
}
