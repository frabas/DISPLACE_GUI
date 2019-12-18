#ifndef WINDFARMSTABLE_H
#define WINDFARMSTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "msqlitecpp/v1/sqlitefielddef.h"
#include "msqlitecpp/v1/sqlitetable.h"

using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Windmill;

class  WindfarmsTable : public SQLiteTable {
    struct Impl;
    std::unique_ptr<Impl> p;

public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldWindfarmId = makeFieldDef("WindarmId", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldWindfarmType = makeFieldDef("WindfarmType", FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> fldKwh = makeFieldDef("Kwh", FieldType::Real());
    const FieldDef<FieldType::Real> fldKwhProduction = makeFieldDef("KwhProduction", FieldType::Real());

    WindfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name);

    ~WindfarmsTable() noexcept;

    void dropAndCreate();

    void exportWindmillData(Windmill *windmill, int tstep);

    struct StatData {
        std::vector<double> t;
        std::vector<double> v;
    };

    enum class StatType {
        Kwh, KwhProduction
    };
    enum class Aggreg {
        Sum, Min, Max, Avg
    };

    StatData getStatData(StatType, Aggreg, int wfType);

    StatData getStatData(StatType, Aggreg);
};

#endif // WINDFARMSTABLE_H
