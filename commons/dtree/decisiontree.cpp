#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>

using namespace dtree;

DecisionTree::DecisionTree()
    : mRoot()
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
//    mNodes.insert(root);
}

void DecisionTree::connect(boost::shared_ptr<Node> node, boost::shared_ptr<Node> parent, int childId)
{
    node->setParent(parent);
    parent->setChild(childId, node);
}

