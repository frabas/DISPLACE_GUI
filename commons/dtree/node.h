#ifndef NODE_H
#define NODE_H

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
    void setExtra(boost::shared_ptr<NodeExtra> extra);
    boost::shared_ptr<NodeExtra> extra() const;

private:
    boost::weak_ptr<DecisionTree> mTree;
    std::vector<boost::shared_ptr<Node> > mNodes;
    boost::shared_ptr<NodeExtra> mExtra;
};

}

#endif // NODE_H
