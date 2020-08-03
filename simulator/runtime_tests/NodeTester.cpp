//
// Created by happycactus on 06/08/19.
//

#include "NodeTester.h"
#include "Node.h"

#include "helpers.h"

struct NodeTester::Impl {
    int mNbPops;
    int mNbBenthosPops;
};

NodeTester::NodeTester(int nbPops, int nbBenthosPops)
        : p(spimpl::make_unique_impl<Impl>())
{
    p->mNbPops = nbPops;
    p->mNbBenthosPops = nbBenthosPops;
}

void NodeTester::test()
{
    // check the class Node
    Node node(types::NodeId(1), 1.0, 1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, p->mNbPops, p->mNbBenthosPops, 5);
    dout(cout << "is the node at 1,1? "
              << node.get_x() << " " << node.get_y() << " " << node.get_is_harbour() << endl);

    if (node.get_x() != 1 || node.get_y() != 1) {
        throw std::logic_error("NodeTester failed.");
    }

    node.set_xy(2, 2);
    dout(cout << "is the node now at 2,2? "
              << node.get_x() << " " << node.get_y() << " " << node.get_is_harbour() << endl);

    if (node.get_x() != 2 || node.get_y() != 2) {
        throw std::logic_error("NodeTester failed.");
    }
}
