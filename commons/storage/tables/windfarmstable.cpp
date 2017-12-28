#include "windfarmstable.h"

#include <Windmill.h>

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

