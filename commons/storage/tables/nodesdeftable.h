#ifndef POPNODESTABLE_H
#define POPNODESTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include <functional>

#include "msqlitecpp/v1/sqlitefielddef.h"
#include "msqlitecpp/v1/sqlitetable.h"

using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class Harbour;

class  NodesDefTable : public SQLiteTable {
private:
    struct Impl;
    std::unique_ptr<Impl> p;

    const FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId", FieldType::Integer()).primaryKey();
    const FieldDef<FieldType::Integer> fldHarbourId = makeFieldDef("HarbourId", FieldType::Integer());
    const FieldDef<FieldType::Text> fldNodeName = makeFieldDef("NodeName", FieldType::Text());
    const FieldDef<FieldType::Real> fldLong = makeFieldDef("Long", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> fldLat = makeFieldDef("Lat", FieldType::Real()).notNull();
    const FieldDef<FieldType::Integer> marineLandscape = makeFieldDef("marineLandscape",
                                                                      FieldType::Integer()).notNull();
    const FieldDef<FieldType::Real> bathymetry = makeFieldDef("bathymetry", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> shippingdensity = makeFieldDef("shippingdensity", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> siltfraction = makeFieldDef("siltfraction", FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> icesrectanglecode = makeFieldDef("icesrectanglecode", FieldType::Real()).notNull();
public:
    NodesDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~NodesDefTable() noexcept;

    void dropAndCreate();

    void insert(Node *node);

    void queryAllNodes(std::function<void(std::shared_ptr<Node>, std::shared_ptr<Harbour>)> operation);
};

#endif // POPNODESTABLE_H
