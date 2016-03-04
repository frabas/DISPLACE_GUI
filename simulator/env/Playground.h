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

        class Playground {
        public:
            explicit Playground();
            ~Playground() noexcept;

            int getNodeCount();
            int addNode ();
            std::shared_ptr<Node> node(int idx);


        private:
            std::unique_ptr<MapGraph> mGraph;
        };

    }
}


#endif //DISPLACE_PLAYGROUND_H
