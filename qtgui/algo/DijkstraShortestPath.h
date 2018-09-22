//
// Created by Federico Fuga on 11/09/18.
//

#ifndef DISPLACE_DIJKSTRASHORTESTPATH_H
#define DISPLACE_DIJKSTRASHORTESTPATH_H

#include "ShortestPathAlgorithm.h"

#include <modelobjects/nodedata.h>

#include <QString>

#include <memory>

class DijkstraShortestPath : public ShortestPathAlgorithm {
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    explicit DijkstraShortestPath(DisplaceModel *model);

    virtual ~DijkstraShortestPath() noexcept;

    void create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                const QList<std::shared_ptr<NodeData> > &relevantNodes) override;

    void saveRelevantNodes (const QList<std::shared_ptr<NodeData> > &, std::function<void(types::NodeId, types::NodeId, double)> writer) override;
};


#endif //DISPLACE_DIJKSTRASHORTESTPATH_H
