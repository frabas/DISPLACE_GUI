#ifndef WINDFARMSTABLE_H
#define WINDFARMSTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Windmill;

class COMMONSSHARED_EXPORT WindfarmsTable : public SQLiteTable
{
public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldWindfarmId = makeFieldDef("WindarmId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldWindfarmType = makeFieldDef("WindfarmType",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> fldKwh = makeFieldDef("Kwh", FieldType::Real());
    const FieldDef<FieldType::Real> fldKwhProduction = makeFieldDef("KwhProduction", FieldType::Real());

    WindfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    void dropAndCreate();

    void exportWindmillData(Windmill *windmill, int tstep);

    struct StatData {
        std::vector<double> t;
        std::vector<double> v;
    };

    enum class StatType { Kwh, KwhProduction };
    enum class Aggreg { Sum, Min, Max, Avg };

    StatData getStatData(StatType, Aggreg, int wfType);
    StatData getStatData(StatType, Aggreg);
};

#endif // WINDFARMSTABLE_H
