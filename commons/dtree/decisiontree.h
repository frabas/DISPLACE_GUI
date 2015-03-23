#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace dtree {

class Node;

class DecisionTree : public boost::enable_shared_from_this<DecisionTree>
{
public:
    DecisionTree();

    boost::shared_ptr<Node> root() const { return mRoot; }
    boost::shared_ptr<Node> createNode() ;

    void setRoot(boost::shared_ptr<Node> root);
    void connect (boost::shared_ptr<Node> node, boost::shared_ptr<Node> parent, int childId);

    bool isEmpty() const { return mRoot.get() == 0; }
private:
    boost::shared_ptr<Node> mRoot;
//    std::set<boost::shared_ptr> mNodes;
};

}

#endif // DECISIONTREE_H
