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

    inline void init();
public:
    NodesEnvtTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~NodesEnvtTable() noexcept;
    void dropAndCreate();

    bool insert (int tstep, Node *node);

    using NodeEnvt = types::EnvironmentData;
    void queryAllNodesAtStep(types::tstep_t tstep, std::function<bool(NodeEnvt stat)>);
    void queryNodeAtStep (types::NodeId nodeId, types::tstep_t tstep, std::function<bool(NodeEnvt stat)>);
};

#endif // NODESENVTTABLE_H
