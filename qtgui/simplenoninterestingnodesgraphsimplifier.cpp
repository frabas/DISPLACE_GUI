#include "simplenoninterestingnodesgraphsimplifier.h"

SimpleNonInterestingNodesGraphSimplifier::SimpleNonInterestingNodesGraphSimplifier()
{

}

bool SimpleNonInterestingNodesGraphSimplifier::operator()(const QList<std::shared_ptr<NodeData> > &relNodes, const ShortestPathBuilder::graph_t &graph, std::vector<ShortestPathBuilder::vertex_descriptor> &predecessors, std::vector<double> &dinstances)
{
    return true;
}

