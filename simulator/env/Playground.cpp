//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Playground.h"
#include "MapGraph.h"

#include <utils/make_unique.h>

using namespace displace::env;

Playground::Playground()
: mGraph(utils::make_unique<MapGraph>())
{

}

Playground::~Playground() noexcept = default;

int Playground::getNodeCount()
{
    return mGraph->nodeCount();
}

int Playground::addNode()
{
    return mGraph->addNode();
}

std::shared_ptr<Node> Playground::node(int idx)
{
    return mGraph->node(idx);
}
