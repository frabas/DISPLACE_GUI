#ifndef VESSELSLOGLIKECATCHESTABLE_H
#define VESSELSLOGLIKECATCHESTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>

class Population;

class COMMONSSHARED_EXPORT VesselsLoglikeCatchesTable : public SQLiteTable
{
public:
    const FieldDef<FieldType::Integer> fldLoglikeId = makeFieldDef("LoglikeId", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Real> fldCatches = makeFieldDef("Catches",FieldType::Real()).notNull();

    VesselsLoglikeCatchesTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    void dropAndCreate();

    void insertPopulation (size_t rowid, int population, const std::vector<double> &catches);
};

#endif // VESSELSLOGLIKECATCHESTABLE_H
