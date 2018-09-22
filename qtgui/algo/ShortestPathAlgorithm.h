//
// Created by Federico Fuga on 22/09/18.
//

#ifndef DISPLACE_SHORTESTPATHALGORITHM_H
#define DISPLACE_SHORTESTPATHALGORITHM_H

#include "idtypes.h"

#include <QList>

#include <memory>

class QString;

class NodeData;

class ShortestPathAlgorithm {
public:
    virtual ~ShortestPathAlgorithm() noexcept = default;

    virtual void create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                        const QList<std::shared_ptr<NodeData> > &relevantNodes) = 0;

    virtual void saveRelevantNodes(const QList<std::shared_ptr<NodeData> > &,
                                   std::function<void(types::NodeId, types::NodeId, double)> writer) = 0;

};

#endif //DISPLACE_SHORTESTPATHALGORITHM_H
