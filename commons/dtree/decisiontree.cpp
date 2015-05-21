#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <set>


using namespace dtree;

DecisionTree::DecisionTree()
    : mRoot(),
      mType(DecisionTreeManager::InvalidTreeType)
{
}

void DecisionTree::clear()
{
    mRoot.reset();
    mType = DecisionTreeManager::InvalidTreeType;
}

boost::shared_ptr<Node> DecisionTree::createNode()
{
    boost::shared_ptr<Node> node(new Node(shared_from_this()));

    return node;
}

void DecisionTree::setRoot(boost::shared_ptr<Node> root)
{
    mRoot = root;
}

void DecisionTree::connect(boost::shared_ptr<Node> node, boost::shared_ptr<Node> parent, int childId)
{
    node->setParent(parent);
    parent->setChild(childId, node);
}

void DecisionTree::removeNodes(std::list<boost::shared_ptr<Node> > nodes)
{
    // collect nodes to remove
    std::list<boost::shared_ptr<Node> > queue;

    for (std::list<boost::shared_ptr<Node> >::iterator it = nodes.begin(); it != nodes.end(); ++it)
        queue.push_back(*it);

    std::set<boost::shared_ptr<Node> > nodes_to_remove;
    while (!queue.empty()) {
        boost::shared_ptr<Node> node = queue.front();
        queue.pop_front();

        nodes_to_remove.insert(node);
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            boost::shared_ptr<dtree::Node> ch = node->getChild(i);
            if (ch) {
                queue.push_back(ch);
            }
        }
    }

    for (std::set<boost::shared_ptr<Node> >::iterator it = nodes_to_remove.begin(); it != nodes_to_remove.end(); ++it) {
        boost::shared_ptr<Node> node = *it;
        boost::shared_ptr<Node> parent = node->parent().lock();
        if (parent) {
            for (int j = 0; j < parent->getChildrenCount(); ++j) {
                if (parent->getChild(j) == node) {
                    parent->setChild(j, boost::shared_ptr<dtree::Node>());
                    break;
                }
            }
            node->setParent(boost::weak_ptr<Node>());
        } else {
            mRoot.reset();
        }
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            node->setChild(i, boost::shared_ptr<Node>());
        }
    }
}
