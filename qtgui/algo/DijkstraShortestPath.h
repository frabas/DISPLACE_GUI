//
// Created by Federico Fuga on 11/09/18.
//

#ifndef DISPLACE_DIJKSTRASHORTESTPATH_H
#define DISPLACE_DIJKSTRASHORTESTPATH_H

#include <modelobjects/nodedata.h>

#include <QString>

#include <memory>

class QTextStream;

class DijkstraShortestPath {
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    explicit DijkstraShortestPath(DisplaceModel *model);

    virtual ~DijkstraShortestPath() noexcept;

    void create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                const QList<std::shared_ptr<NodeData> > &relevantNodes);

    void saveRelevantNodes (const QList<std::shared_ptr<NodeData> > &, std::function<void(types::NodeId, types::NodeId, double)> writer);
};


#endif //DISPLACE_DIJKSTRASHORTESTPATH_H
