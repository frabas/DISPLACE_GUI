#ifndef DTNODE_H
#define DTNODE_H

#include <dtree/variables.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>

namespace dtree {

class DecisionTree;
class NodeExtra;

/** \brief a decision tree node.
 *
 */
class Node
{
public:
    explicit Node(boost::shared_ptr<DecisionTree> node);

    int getChildrenCount() const;

    /** \brief Returns the children pointer indexed by the argument
     * If the children has been remapped using setMapping(), it returns the remapped pointer.
     */
    boost::shared_ptr<Node> getChild(int idx);

    /** \brief Returns the children pointer indexed by the argument, ignoring any mapping. */
    boost::shared_ptr<Node> getUnmappedChild(int idx);
    void setChild(int idx, boost::shared_ptr<Node> child);

    void setMapping (int idx, int remapped);

    void setExtra(boost::shared_ptr<NodeExtra> extra);
    boost::shared_ptr<NodeExtra> extra() const;

    Variable variable() const { return mVariable; }
    void setVariable(Variable var);

    void setParent(boost::weak_ptr<Node> node) { mParent = node; }
    boost::weak_ptr<Node> parent() const { return mParent; }

    double value() const { return mValue; }
    void setValue(double v) { mValue = v; }

private:
    boost::weak_ptr<DecisionTree> mTree;
    boost::weak_ptr<Node> mParent;
    std::vector<boost::shared_ptr<Node> > mNodes;
    std::vector<int> mGroups;
    boost::shared_ptr<NodeExtra> mExtra;

    Variable mVariable;
    double mValue;
};

}

#endif // NODE_H
