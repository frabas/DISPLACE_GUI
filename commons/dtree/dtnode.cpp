#include <dtree/dtnode.h>

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

void Node::setChild(int idx, boost::shared_ptr<Node> child)
{
    mNodes[idx] = child;
}

void Node::setExtra(boost::shared_ptr<NodeExtra> extra)
{
    mExtra = extra;
}

boost::shared_ptr<NodeExtra> Node::extra() const
{
    return mExtra;
}

void Node::setVariable(Variable var)
{
    if (mVariable != var) {
        mNodes.clear();
        // Note: binary node
        if (var != VarUndefined) {
            mNodes.push_back(boost::shared_ptr<Node> ());
            mNodes.push_back(boost::shared_ptr<Node> ());
        }
        mVariable = var;
    }
}
