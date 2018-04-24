#ifndef SIMPLENONINTERESTINGNODESGRAPHSIMPLIFIER_H
#define SIMPLENONINTERESTINGNODESGRAPHSIMPLIFIER_H

#include <Node.h>
#include <shortestpathbuilder.h>

#include <QList>

#include <memory>

class SimpleNonInterestingNodesGraphSimplifier
{
public:
    SimpleNonInterestingNodesGraphSimplifier();

    bool operator() (const QList<std::shared_ptr<NodeData> > &relNodes,
                     const ShortestPathBuilder::graph_t &graph,
                     std::vector<ShortestPathBuilder::vertex_descriptor> &predecessors,
                     std::vector<double> &dinstances);
};

#endif // SIMPLENONINTERESTINGNODESGRAPHSIMPLIFIER_H
