#ifndef POPNODESTABLE_H
#define POPNODESTABLE_H

#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class PopNodesTable : public SQLiteTable
{
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> fldLong = makeFieldDef("Long",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldLat = makeFieldDef("Lat",FieldType::Real()).notNull();

public:
    PopNodesTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    void insert(Node* node);
};

#endif // POPNODESTABLE_H
