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

class COMMONSSHARED_EXPORT NodesDefTable : public SQLiteTable
{
    const FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Text> fldNodeName = makeFieldDef("NodeName",FieldType::Text());
    const FieldDef<FieldType::Real> fldLong = makeFieldDef("Long",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> fldLat = makeFieldDef("Lat",FieldType::Real()).notNull();

public:
    NodesDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    void dropAndCreate();

    void insert(Node* node);
};

#endif // POPNODESTABLE_H
