#ifndef POPSTATTABLE_H
#define POPSTATTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT NodesStatTable : public SQLiteTable
{
private:
    struct Impl;

    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> cumFTime = makeFieldDef("CumFTime",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumSwA = makeFieldDef("CumSwA",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumSubSurfSwA = makeFieldDef("CumSubSurfSwA",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumCatches = makeFieldDef("CumCatches",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumDisc = makeFieldDef("CumDiscards",FieldType::Real()).notNull();

    inline void init();
public:
    NodesStatTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~NodesStatTable() noexcept;
    void dropAndCreate();

    void insert (int tstep, Node *node);

    struct NodeStat {
        types::NodeId nodeId;
        int tstep;
        double cumftime;
        double cumswa;
        double cumsubsurfswa;
        double cumcatches;
        double cumdisc;
    };
    void queryAllNodesAtStep(int tstep, std::function<bool(NodeStat stat)>);
};

#endif // POPSTATTABLE_H
