//
// Created by Federico Fuga on 11/07/18.
//

#ifndef GEOGRAPH_H
#define GEOGRAPH_H

#include <commons_global.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>

#include <random>
#include <vector>
#include <ctime>

class COMMONSSHARED_EXPORT GeoGraph {
public:
    // auxiliary types
    struct location
    {
        float y, x; // lat, long
    };

    using cost = float;

    using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property,
            boost::property<boost::edge_weight_t, cost> >;

    using WeightMap = boost::property_map<Graph, boost::edge_weight_t>::type;
    using vertex = Graph::vertex_descriptor;
    using edge_descriptor = Graph::edge_descriptor;
    using edge = std::pair<int, int>;

    Graph graph;
    WeightMap weightmap = get(boost::edge_weight, graph);
    std::vector<location> locations;

    std::mt19937 randomGenerator = std::mt19937(time(0));

    GeoGraph() = default;

    void addNode(int node, float x, float y);
    bool addEdge(int node_from, int node_to, cost weight);

    auto selectRandomNode () {
        return random_vertex(graph, randomGenerator);
    }

    size_t numNodes() const { return boost::num_vertices(graph); }
    size_t numEdges() const { return boost::num_edges(graph); }
};

extern COMMONSSHARED_EXPORT GeoGraph geoGraph;

#endif //GEOGRAPH_H
