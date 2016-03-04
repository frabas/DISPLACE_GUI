//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "MapGraph.h"

#include <env/Node.h>

using namespace displace::env;

MapGraph::MapGraph()
{

}

MapGraph::~MapGraph() noexcept  = default;

long MapGraph::addNode()
{
    long newname = nodeCount();

    using namespace boost::graph;
    auto nv = add_vertex(mGraph);

    auto nn = std::make_shared<Node>(newname);
    mGraph[nv] = nn;

    return newname;
}
