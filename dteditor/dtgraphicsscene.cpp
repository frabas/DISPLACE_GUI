#include "dtgraphicsscene.h"
#include <dtree/decisiontree.h>

#include <QGraphicsSceneMouseEvent>
#include <graphnodeitem.h>
#include <QDebug>

DtGraphicsScene::DtGraphicsScene(boost::shared_ptr<dtree::DecisionTree> tree, QObject *parent) :
    QGraphicsScene(parent),
    mTree(tree),
    mRoot(0),
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

void DtGraphicsScene::addItemAsRoot(GraphNodeItem *item)
{
    addItem(item);
    mRoot = item;
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
        if (!mTree->isEmpty()) {
            addItem(mAddingItem);
            mMode = AddNodeConnect;
        } else {
            addItemAsRoot(mAddingItem);
            mRoot = mAddingItem;
            mTree->setRoot(mAddingNode);
            mAddingNode.reset();
            endMode();
        }
        mAddingItem->setPos(event->scenePos());
        break;
    case AddNodeConnect:
        // connect the nodes
        boost::shared_ptr<dtree::Node> nd;
        if (mHoveringNode != 0 && mHoveringNodeChild != -1 && ( nd = mHoveringNode->getNode()).get() != 0 && nd != mAddingNode) {
            mTree->connect(mAddingNode, nd, mHoveringNodeChild);
            mAddingItem->connectAsChild(mHoveringNode, mHoveringNodeChild);
            mAddingItem->update();
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
