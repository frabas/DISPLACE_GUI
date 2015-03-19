#ifndef DECISIONTREE_H
#define DECISIONTREE_H

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

private:
    boost::shared_ptr<Node> mRoot;
};

}

#endif // DECISIONTREE_H
