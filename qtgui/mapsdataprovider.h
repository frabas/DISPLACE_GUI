#ifndef MAPSDATAPROVIDER_H
#define MAPSDATAPROVIDER_H

#include <memory>
#include "modeltypes.h"

class SQLiteOutputStorage;

class MapsDataProvider
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    MapsDataProvider();
    ~MapsDataProvider() noexcept;

    void attach (std::shared_ptr<SQLiteOutputStorage> storage);

    std::shared_ptr<types::EnvironmentData> getEnvironmentData (types::NodeId nodeId, types::tstep_t tstep);
    std::shared_ptr<types::NodesPopData> getNodesPopData(types::NodeId nodeId, types::tstep_t tstep);
    std::shared_ptr<types::NodesBenthosData> getNodesBenthosData (types::NodeId nodeId, types::tstep_t tstep);
};

#endif // MAPSDATAPROVIDER_H
