#ifndef POPTARIFFSTATTABLE_H
#define POPTARIFFSTATTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT NodesTariffStatTable : public SQLiteTable
{
private:
    struct Impl;

    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> tariffAll = makeFieldDef("TariffAll",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> tariffPop = makeFieldDef("TariffPop",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> tariffBenthos = makeFieldDef("TariffBenthos",FieldType::Real()).notNull();

    inline void init();
public:
    NodesTariffStatTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~NodesTariffStatTable() noexcept;
    void dropAndCreate();

    bool insert (int tstep, Node *node);

    struct NodeTariffStat {
        types::NodeId nodeId;
        int tstep;
        double tariffall;
        double tariffpop;
        double tariffbenthos;
    };
    void queryAllNodesAtStep(int tstep, std::function<bool(NodeTariffStat stat)>);
};

#endif // POPTARIFFSTATTABLE_H
