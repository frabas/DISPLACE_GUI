#ifndef DTNODE_H
#define DTNODE_H

#include <commons_global.h>
#include <dtree/variables.h>

#include <memory>
#include <boost/weak_ptr.hpp>
#include <vector>

namespace dtree {

class DecisionTree;
class NodeExtra;

/** \brief a decision tree node.
 *
 */
class COMMONSSHARED_EXPORT Node
{
public:
    explicit Node(std::shared_ptr<DecisionTree> node);

    int getChildrenCount() const;

    /** \brief Returns the children pointer indexed by the argument
     * If the children has been remapped using setMapping(), it returns the remapped pointer.
     */
    std::shared_ptr<Node> getChild(int idx);

    /** \brief Returns the children pointer indexed by the argument, ignoring any mapping. */
    std::shared_ptr<Node> getUnmappedChild(int idx);
    void setChild(int idx, std::shared_ptr<Node> child);

    void setMapping (int idx, int remapped);
    int getMapping(int idx) const;

    void setExtra(std::shared_ptr<NodeExtra> extra);
    std::shared_ptr<NodeExtra> extra() const;

    Variable variable() const { return mVariable; }
    void setVariable(Variable var);

    void setParent(std::weak_ptr<Node> node) { mParent = node; }
    std::weak_ptr<Node> parent() const { return mParent; }

    double value() const { return mValue; }
    void setValue(double v) { mValue = v; }

private:
    std::weak_ptr<DecisionTree> mTree;
    std::weak_ptr<Node> mParent;
    std::vector<std::shared_ptr<Node> > mNodes;
    std::vector<int> mGroups;
    std::shared_ptr<NodeExtra> mExtra;

    Variable mVariable;
    double mValue;
};

}

#endif // NODE_H
