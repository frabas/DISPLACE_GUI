//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_PLAYGROUND_H
#define DISPLACE_PLAYGROUND_H

#include <memory>

namespace displace {
    namespace env {

        class MapGraph;
        class Node;
        class Edge;

        class Playground {
        public:
            explicit Playground();
            ~Playground() noexcept;

            int getNodeCount() const;
            int addNode ();
            std::shared_ptr<Node> node(int idx);

            int getEdgeCount() const;
            int addEdge (int from, int to);
            std::shared_ptr<Edge> edge(int from, int to);

        private:
            std::unique_ptr<MapGraph> mGraph;
        };

    }
}


#endif //DISPLACE_PLAYGROUND_H
