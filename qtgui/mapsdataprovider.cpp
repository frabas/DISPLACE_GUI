#include "mapsdataprovider.h"
#include "storage/tables/nodesenvttable.h"


namespace {
using EnvPtr = std::shared_ptr<types::EnvironmentData>;

struct CachedEnvironmentData {
    bool dirty = true;
    types::tstep_t refTstep;
    EnvPtr data;
};
}

struct MapsDataProvider::Impl
{
    std::shared_ptr<SQLiteOutputStorage> db;
    std::shared_ptr<NodesEnvtTable> envTable;

    std::vector<CachedEnvironmentData> environmentData;

    CachedEnvironmentData &getEnvironmentData(types::NodeId nodeId) {
        while (environmentData.size() < nodeId.toIndex())
            environmentData.push_back(CachedEnvironmentData());
        return environmentData.at(nodeId.toIndex());
    }
};

MapsDataProvider::MapsDataProvider()
    : p(std::make_unique<Impl>())
{

}

MapsDataProvider::~MapsDataProvider() noexcept = default;

void MapsDataProvider::attach(std::shared_ptr<SQLiteOutputStorage> storage)
{
    if (p->db == storage)
        return;

    p->db = storage;
    p->environmentData.clear();

    p->envTable = p->db->getNodesEnvtTable();
}

std::shared_ptr<types::EnvironmentData> MapsDataProvider::getEnvironmentData(types::NodeId nodeId, types::tstep_t tstep)
{
    auto &d = p->getEnvironmentData(nodeId);

    // it's clean. pass.
    if (!d.dirty && d.refTstep == tstep)
        return d.data;

    // not clean: update
    d.data = nullptr;
    p->envTable->queryNodeAtStep(nodeId, tstep, [&d, &tstep](NodesEnvtTable::NodeEnvt n) {
        d.dirty = false;
        d.refTstep = tstep;
        d.data = std::make_shared<types::EnvironmentData>(n);
        return true;
    });

    return d.data;
}

