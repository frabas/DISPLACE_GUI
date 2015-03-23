#include "decisiontree.h"
#include <dtree/node.h>

using namespace dtree;

DecisionTree::DecisionTree()
{
}

boost::shared_ptr<Node> DecisionTree::createNode()
{
    boost::shared_ptr<Node> node(new Node(shared_from_this()));

    return node;
}

void DecisionTree::setRoot(boost::shared_ptr<Node> root)
{
    mRoot = root;
}

