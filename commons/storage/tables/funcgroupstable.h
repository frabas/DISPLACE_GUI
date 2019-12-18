#ifndef FUNCGROUPSTABLE_H
#define FUNCGROUPSTABLE_H

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

class  FuncGroupsTable : public SQLiteTable {
private:
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    const FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldFGroup = makeFieldDef("FuncGroup", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> fldBType = makeFieldDef("BenthosType", FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> benthosNumTot = makeFieldDef("bNTot", FieldType::Real());
    const FieldDef<FieldType::Real> benthosBio = makeFieldDef("bB", FieldType::Real());
    const FieldDef<FieldType::Real> benthosMeanW = makeFieldDef("bMeanW", FieldType::Real());
    const FieldDef<FieldType::Real> benthosBioK = makeFieldDef("bBK", FieldType::Real());
    const FieldDef<FieldType::Real> benthosNumK = makeFieldDef("bNK", FieldType::Real());

    FuncGroupsTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~FuncGroupsTable() noexcept;

    void create();

    void dropAndCreate();

    void insert(int tstep, Node *node, int funcgr, int isN);

    struct Stat {
        int tstep;
        types::NodeId nodeId;
        int funcId;

        double numTot, bio, bioMeanW, bioK, numK;
    };

    void queryAllNodesAtStep(int tstep, std::function<bool(Stat)> op);
};

#endif // FUNCGROUPSTABLE_H
