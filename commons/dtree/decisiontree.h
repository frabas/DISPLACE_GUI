#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <dtree/decisiontreemanager.h>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace dtree {

class Node;

class DecisionTree : public boost::enable_shared_from_this<DecisionTree>
{
public:
    DecisionTree();

    void clear();

    boost::shared_ptr<Node> root() const { return mRoot; }
    boost::shared_ptr<Node> createNode() ;

    void setRoot(boost::shared_ptr<Node> root);
    void connect (boost::shared_ptr<Node> node, boost::shared_ptr<Node> parent, int childId);

    bool isEmpty() const { return mRoot.get() == 0; }

    void setType(DecisionTreeManager::TreeType t) {
        mType = t;
    }
    DecisionTreeManager::TreeType type() const { return mType; }

private:
    boost::shared_ptr<Node> mRoot;

    DecisionTreeManager::TreeType mType;
};

}

#endif // DECISIONTREE_H
