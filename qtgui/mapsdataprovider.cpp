#include "mapsdataprovider.h"
#include "storage/tables/nodesenvttable.h"


namespace {
using EnvPtr = std::shared_ptr<types::EnvironmentData>;

struct CachedEnvironmentData {
    bool dirty = true;
    types::tstep_t cachedTstep;
    EnvPtr data;
};
}

struct MapsDataProvider::Impl
{
    std::shared_ptr<SQLiteOutputStorage> db;
    std::shared_ptr<NodesEnvtTable> envTable;

    std::vector<CachedEnvironmentData> environmentData;

    CachedEnvironmentData &getEnvironmentData(types::NodeId nodeId) {
        while (environmentData.size() <= nodeId.toIndex())
            environmentData.push_back(CachedEnvironmentData());
        return environmentData.at(nodeId.toIndex());
    }

    void updateEnvtData (types::tstep_t step, NodesEnvtTable::NodeEnvt n) {
        auto &d = getEnvironmentData(n.nodeId);
        d.dirty = false;
        d.cachedTstep = step;
        d.data = std::make_shared<types::EnvironmentData>(std::move(n));
    }

    void refreshAllData (types::tstep_t step) {
        for(auto &e : environmentData) {
            e.data.reset();
            e.dirty = false;
            e.cachedTstep = step;
        }

        std::cout << "Evt: Started refresh (" << step.value() << ")\n";
        size_t count = 0;
        auto n = std::chrono::system_clock::now();
        envTable->queryAllNodesAtStep(step,[this,step,&count](NodesEnvtTable::NodeEnvt n){
            updateEnvtData(step, n);
            ++count;
            return true;
        } );

        std::cout << "Refresh All EVT data query: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now() - n
                         ).count() << "ms " << "(" << count << " results)\n";
    }

    void refreshDataForNode (types::NodeId nodeId, types::tstep_t step) {
        std::cout << "Refresh data for node " << nodeId.toIndex() << " tstep " << step.value() << "\n";
        envTable->queryNodeAtStep(nodeId, step, [&step, this](NodesEnvtTable::NodeEnvt n) {
            updateEnvtData(step, n);
            return true;
        });

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
    // not attached. No data.
    if (p->db == nullptr)
        return nullptr;

    auto &d = p->getEnvironmentData(nodeId);

    // it's clean. pass.
    if (!d.dirty && d.cachedTstep == tstep)
        return d.data;

    p->refreshAllData(tstep);
    // the following code would be better but it's not optimal.
    //p->refreshDataForNode(nodeId, tstep);

    return d.data;
}

