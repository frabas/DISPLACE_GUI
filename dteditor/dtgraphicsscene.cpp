#include "dtgraphicsscene.h"
#include <dtree/decisiontree.h>

#include <QGraphicsSceneMouseEvent>
#include <graphnodeitem.h>
#include <QDebug>

DtGraphicsScene::DtGraphicsScene(boost::shared_ptr<dtree::DecisionTree> tree, QObject *parent) :
    QGraphicsScene(parent),
    mTree(tree),
    mAddingNode(),
    mAddingItem(),
    mHoveringNode(0),
    mHoveringNodeChild(-1)
{
}

bool DtGraphicsScene::requiresChildrenHighlight() const
{
    return mMode == AddNodeConnect;
}

void DtGraphicsScene::nodeChildEntered(GraphNodeItem *item, int childId)
{
    mHoveringNode = item;
    mHoveringNodeChild = childId;
}

void DtGraphicsScene::nodeChildExited()
{
    mHoveringNode = 0;
    mHoveringNodeChild = -1;
}

#if 0
void DtGraphicsScene::addUnconnectedNode(boost::shared_ptr<dtree::Node> node, QPointF pos)
{
    GraphNodeItem *item = new GraphNodeItem(node);
    item->setPos(pos);

    mUnconnectedNodes.insert(node);
    addItem(item);
}
#endif

void DtGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (mMode) {
    case AddNodeConnect:
        mAddingItem->moveArrow(event->scenePos());
        break;
    default:
        break;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void DtGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (mMode) {
    case None:
        break;
    case AddNode:
        mAddingItem->setPos(event->scenePos());
        addItem(mAddingItem);

        if (!mTree->isEmpty()) {
            mMode = AddNodeConnect;
        } else {
            mRoot = mAddingItem;
            mTree->setRoot(mAddingNode);
            mAddingNode.reset();
            endMode();
        }
        break;
    case AddNodeConnect:
        // connect the nodes
        boost::shared_ptr<dtree::Node> nd;
        if (mHoveringNode != 0 && mHoveringNodeChild != -1 && ( nd = mHoveringNode->getNode()).get() != 0 && nd != mAddingNode) {
            mTree->connect(mAddingNode, nd, mHoveringNodeChild);
            mAddingItem->connectAsChild(mHoveringNode, mHoveringNodeChild);
            mAddingNode.reset();
            endMode();
        }
        break;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void DtGraphicsScene::startAddNode(boost::shared_ptr<dtree::Node> node)
{
    mAddingItem = new GraphNodeItem(node, this);
    mMode = AddNode;
    mAddingNode = node;
}

void DtGraphicsScene::endMode()
{
    mMode = None;
    emit mouseModeEnded();
}
