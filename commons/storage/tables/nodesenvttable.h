#ifndef NODESENVTTABLE_H
#define NODESENVTTABLE_H

#include "commons_global.h"
#include "idtypes.h"
#include "modeltypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT NodesEnvtTable : public SQLiteTable
{
private:
    struct Impl;

    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Integer> marineLandscape = makeFieldDef("marineLandscape",FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> salinity = makeFieldDef("salinity",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> sst = makeFieldDef("sst",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> wind = makeFieldDef("wind",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> nitrogen = makeFieldDef("nitrogen",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> phosphorus = makeFieldDef("phosphorus",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> oxygen = makeFieldDef("oxygen",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> dissolvedcarbon = makeFieldDef("dissolvedcarbon",FieldType::Real()).notNull();

    inline void init();
public:
    NodesEnvtTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~NodesEnvtTable() noexcept;
    void dropAndCreate();

    bool insert (int tstep, Node *node);

    using NodeEnvt = types::EnvironmentData;
    void queryAllNodesAtStep(int tstep, std::function<bool(NodeEnvt stat)>);
};

#endif // NODESENVTTABLE_H
