#ifndef POPTARIFFSTATTABLE_H
#define POPTARIFFSTATTABLE_H

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

class  NodesTariffStatTable : public SQLiteTable {
private:
    struct Impl;

    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId", FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> tariffMet0 = makeFieldDef("TariffMet0", FieldType::Real()).notNull();
    FieldDef<FieldType::Real> tariffMet1 = makeFieldDef("TariffMet1", FieldType::Real()).notNull();
    FieldDef<FieldType::Real> tariffMet2 = makeFieldDef("TariffMet2", FieldType::Real()).notNull();

    inline void init();

public:
    NodesTariffStatTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~NodesTariffStatTable() noexcept;

    void dropAndCreate();

    bool insert(int tstep, Node *node);

    struct NodeTariffStat {
        types::NodeId nodeId;
        int tstep;
        double tariffmet0;
        double tariffmet1;
        double tariffmet2;
    };

    void queryAllNodesAtStep(int tstep, std::function<bool(NodeTariffStat stat)>);
};

#endif // POPTARIFFSTATTABLE_H
