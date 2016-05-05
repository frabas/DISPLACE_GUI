#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <dtree/decisiontreemanager.h>
#include <list>
#include <memory>

namespace dtree {

class Node;

class DecisionTree : public std::enable_shared_from_this<DecisionTree>
{
public:
    DecisionTree();

    void clear();

    std::shared_ptr<Node> root() const { return mRoot; }
    std::shared_ptr<Node> createNode() ;

    void setRoot(std::shared_ptr<Node> root);
    void connect (std::shared_ptr<Node> node, std::shared_ptr<Node> parent, int childId);

    bool isEmpty() const { return mRoot.get() == 0; }

    void setType(DecisionTreeManager::TreeType t) {
        mType = t;
    }
    DecisionTreeManager::TreeType type() const { return mType; }

    void removeNodes(std::list<std::shared_ptr<Node> > nodes);
private:
    std::shared_ptr<Node> mRoot;

    DecisionTreeManager::TreeType mType;
};

}

#endif // DECISIONTREE_H
