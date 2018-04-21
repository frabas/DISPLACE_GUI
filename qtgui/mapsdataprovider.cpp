#include "mapsdataprovider.h"
#include "storage/tables/nodesenvttable.h"

#include "storage/cacheddatastorage.h"

namespace {
class EnvironmentCachedData : public CachedDataStorage<types::EnvironmentData> {
    NodesEnvtTable &table;
public:
    EnvironmentCachedData(NodesEnvtTable &t) : table(t) {}
    void queryAllData(types::tstep_t step) override {
        table.queryAllNodesAtStep(step,[this,step](NodesEnvtTable::NodeEnvt n){
            updateData(step, n);
            return true;
        } );
    }
};

}

struct MapsDataProvider::Impl
{
    std::shared_ptr<SQLiteOutputStorage> db;
    std::shared_ptr<NodesEnvtTable> envTable;

    std::shared_ptr<EnvironmentCachedData> environmentData;
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
    p->envTable = p->db->getNodesEnvtTable();

    p->environmentData = std::make_shared<EnvironmentCachedData>(*p->envTable);
    p->environmentData->invalidateAllCache();
}

std::shared_ptr<types::EnvironmentData> MapsDataProvider::getEnvironmentData(types::NodeId nodeId, types::tstep_t step)
{
    // not attached. No data.
    if (p->db == nullptr)
        return nullptr;
    return p->environmentData->getData(nodeId, step);
}

