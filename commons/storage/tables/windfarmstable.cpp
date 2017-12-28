#include "windfarmstable.h"

#include <Windmill.h>
#include <sqlitestatement.h>

WindfarmsTable::WindfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{
}

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
    insert(fldTStep.assign(tstep),
           fldWindfarmId.assign(windmill->get_idx()),
           fldWindfarmType.assign(windmill->get_type()),
           fldKwh.assign(windmill->get_kWh()),
           fldKwhProduction.assign(windmill->get_kW_production())
           );
}

WindfarmsTable::StatData WindfarmsTable::getStatData(WindfarmsTable::StatType stattype, WindfarmsTable::Aggreg aggregtyp)
{
    std::string dtt;
    switch (stattype) {
    case WindfarmsTable::StatType::Kwh:
        dtt = fldKwh.name(); break;
    case WindfarmsTable::StatType::KwhProduction:
        dtt = fldKwhProduction.name(); break;
    }

    std::ostringstream ss;
    ss << "SELECT TStep,SUM(" << dtt << ") FROM " << name() << " GROUP BY TStep";
    std::cout << "Windmill Statement: " << ss.str() << "\n";

    sqlite::SQLiteStatement smt (db(), ss.str());

    WindfarmsTable::StatData d;

    while (smt.execute([&d, &smt]() {
        auto tstep = smt.getIntValue(1);
        auto v = smt.getDoubleValue(2);
        d.t.push_back(tstep);
        d.v.push_back(v);

        return true;
    }));

    std::cout << "Statement returned " << d.t.size() << " values\n";

    return d;
}

WindfarmsTable::StatData WindfarmsTable::getStatData(WindfarmsTable::StatType stattype, WindfarmsTable::Aggreg aggreg, int wfType)
{
    std::string dtt;
    switch (stattype) {
    case WindfarmsTable::StatType::Kwh:
        dtt = fldKwh.name(); break;
    case WindfarmsTable::StatType::KwhProduction:
        dtt = fldKwhProduction.name(); break;
    }

    std::ostringstream ss;
    ss << "SELECT TStep,SUM(" << dtt << ") FROM " << name() << " WHERE " << fldWindfarmType.name() << " = ? GROUP BY TStep";

    std::cout << "Windmill Statement: " << ss.str() << "\n";

    sqlite::SQLiteStatement smt (db(), ss.str());
    smt.bind(1, wfType);

    WindfarmsTable::StatData d;
    smt.execute([&d, &smt]() {
        auto tstep = smt.getIntValue(1);
        auto v = smt.getDoubleValue(2);
        d.t.push_back(tstep);
        d.v.push_back(v);

        return true;
    });

    return d;
}

