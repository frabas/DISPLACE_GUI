#include <dtree/dtnode.h>

using namespace dtree;

Node::Node(boost::shared_ptr<DecisionTree> node)
    : mTree(node),
      mNodes(),
      mExtra(),
      mVariable(VarUndefined),
      mValue(0.0)
{
}

int Node::getChildrenCount() const
{
    return mNodes.size();
}

boost::shared_ptr<Node> Node::getChild(int idx)
{
    return mNodes.at(mGroups[idx]);
}

boost::shared_ptr<Node> Node::getUnmappedChild(int idx)
{
    return mNodes.at(idx);
}

void Node::setChild(int idx, boost::shared_ptr<Node> child)
{
    mNodes[idx] = child; /*VariableNames::Bin*/
}

void Node::setMapping(int idx, int remapped)
{
    if (idx != remapped)
        mNodes[idx].reset();
    mGroups[idx] = remapped;
}

int Node::getMapping(int idx) const
{
    return mGroups[idx];
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
        mGroups.clear();
        // Note: binary node
        for (int i = 0; i < VariableNames::variableBinCount(var); ++i) {
            mNodes.push_back(boost::shared_ptr<Node> ());
            mGroups.push_back(i);
        }
        mVariable = var;
    }
}
