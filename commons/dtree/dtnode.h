#ifndef NODE_H
#define NODE_H

#include <dtree/variables.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>

namespace dtree {

class DecisionTree;
class NodeExtra;

class Node
{
public:
    explicit Node(boost::shared_ptr<DecisionTree> node);

    int getChildrenCount() const;
    boost::shared_ptr<Node> getChild(int idx);
    void setChild(int idx, boost::shared_ptr<Node> child);

    void setExtra(boost::shared_ptr<NodeExtra> extra);
    boost::shared_ptr<NodeExtra> extra() const;

    Variable variable() const { return mVariable; }
    void setVariable(Variable var) { mVariable = var; }

    void setParent(boost::weak_ptr<Node> node) { mParent = node; }
    boost::weak_ptr<Node> parent() const { return mParent; }

private:
    boost::weak_ptr<DecisionTree> mTree;
    boost::weak_ptr<Node> mParent;
    std::vector<boost::shared_ptr<Node> > mNodes;
    boost::shared_ptr<NodeExtra> mExtra;

    Variable mVariable;
};

}

#endif // NODE_H