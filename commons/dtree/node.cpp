#include "node.h"

using namespace dtree;

Node::Node(boost::shared_ptr<DecisionTree> node)
    : mTree(node),
      mNodes(),
      mExtra(),
      mVariable(VarUndefined)
{
}

int Node::getChildrenCount() const
{
    return mNodes.size();
}

boost::shared_ptr<Node> Node::getChild(int idx)
{
    return mNodes.at(idx);
}

void Node::setExtra(boost::shared_ptr<NodeExtra> extra)
{
    mExtra = extra;
}

boost::shared_ptr<NodeExtra> Node::extra() const
{
    return mExtra;
}
