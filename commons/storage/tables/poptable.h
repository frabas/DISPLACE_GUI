#ifndef POPTABLE_H
#define POPTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT PopTable : public SQLiteTable
{
    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> fldTotNId = makeFieldDef("TotalN",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldTotWId = makeFieldDef("TotalW",FieldType::Real()).notNull();

public:
    PopTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    void insert (int tstep, Node *node, const std::multimap<int, double> &initstate);
};

#endif // POPTABLE_H
