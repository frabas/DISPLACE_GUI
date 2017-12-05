#ifndef POPNODESTABLE_H
#define POPNODESTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT PopNodesTable : public SQLiteTable
{
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> fldLong = makeFieldDef("Long",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldLat = makeFieldDef("Lat",FieldType::Real()).notNull();

public:
    PopNodesTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    void insert(Node* node);
};

#endif // POPNODESTABLE_H
