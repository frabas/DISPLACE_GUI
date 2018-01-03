#include "vesselsloglikecatchestable.h"

VesselsLoglikeCatchesTable::VesselsLoglikeCatchesTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
}

void VesselsLoglikeCatchesTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(
               fldLoglikeId,
               fldPopId,
               fldCatches
    ));
}

