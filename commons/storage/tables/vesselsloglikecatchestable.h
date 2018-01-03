#ifndef VESSELSLOGLIKECATCHESTABLE_H
#define VESSELSLOGLIKECATCHESTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>

class COMMONSSHARED_EXPORT VesselsLoglikeCatchesTable : public SQLiteTable
{
    FieldDef<FieldType::Integer> fldLoglikeId = makeFieldDef("LoglikeId", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> fldCatches = makeFieldDef("Catches",FieldType::Real()).notNull();

public:
    VesselsLoglikeCatchesTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    void dropAndCreate();

};

#endif // VESSELSLOGLIKECATCHESTABLE_H
