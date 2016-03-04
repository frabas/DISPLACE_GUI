//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_MAPGRAPH_H
#define DISPLACE_MAPGRAPH_H

#include <boost/graph/adjacency_list.hpp>

#include <memory>

namespace displace {
    namespace env {

        class Node;
        class Edge;

        class MapGraph {
        public:
            explicit MapGraph();

            ~MapGraph() noexcept;

            unsigned long nodeCount() const {
                return mGraph.m_vertices.size();
            }
            long addNode();
            std::shared_ptr<Node> node(int idx) const {
                return mGraph[idx];
            }

            unsigned long edgeCount() const {
                return mGraph.m_edges.size();
            }
            long addEdge(int src, int to);
            std::shared_ptr<Edge> edge(int from, int to) {
                boost::graph_traits<Graph>::edge_descriptor e;
                bool b;
                std::tie(e,b) = boost::edge(from, to, mGraph);

                if (b) {
                    return mGraph[e];
                }
                return nullptr;
            }

            using NodeProperty = std::shared_ptr<Node>;
            using EdgeProperty = std::shared_ptr<Edge>;

        private:
            using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                    NodeProperty, EdgeProperty>;

            Graph mGraph;
        };

    }
}

#endif //DISPLACE_MAPGRAPH_H
