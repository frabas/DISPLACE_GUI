#ifndef POPDYNTABLE_H
#define POPDYNTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Population;

class COMMONSSHARED_EXPORT PopDynTable : public SQLiteTable
{
public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> fldN = makeFieldDef("N",FieldType::Real());
    const FieldDef<FieldType::Real> fldF = makeFieldDef("F",FieldType::Real());
    const FieldDef<FieldType::Real> fldSSB = makeFieldDef("SSB",FieldType::Real());

    PopDynTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    void dropAndCreate();
    void insert (int tstep, Population *pop);
};

#endif // POPDYNTABLE_H
