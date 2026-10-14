

#include "GeoGraph.h"

#include <boost/graph/astar_search.hpp>


using namespace boost;

void GeoGraph::addNode(int node, float x, float y)
{
    while (locations.size() <= node) {
        locations.push_back(location{});
    }
    locations[node] = {y, x};
}

bool GeoGraph::addEdge(int node_from, int node_to, GeoGraph::cost weight)
{
    edge_descriptor e;
    bool inserted;

    std::tie(e, inserted) = add_edge(node_from, node_to, graph);
    weightmap[e] = weight;

//   std::cout << "Addedge: " << node_from << " to " << node_to << " w " << weight << " => " << inserted << "\n";

    return inserted;
}
