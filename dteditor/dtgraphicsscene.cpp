#include "dtgraphicsscene.h"
#include <dtree/decisiontree.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
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

void DtGraphicsScene::clear()
{
    mRoot = 0;
    mAddingNode.reset();
    if (mAddingItem) delete mAddingItem;
    mAddingItem = 0;
    mHoveringNode = 0;
    mHoveringNodeChild = -1;

    QGraphicsScene::clear();
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
            mAddingItem->setPos(event->scenePos());
            mMode = AddNodeConnect;
        } else {
            addItemAsRoot(mAddingItem);
            mAddingItem->setPos(event->scenePos());
            mRoot = mAddingItem;
            mTree->setRoot(mAddingNode);
            emit nodeAdded(mAddingItem);
            mAddingNode.reset();
            mAddingItem = 0;
            endMode();
        }
        return;
    case AddNodeConnect:
        // connect the nodes
        boost::shared_ptr<dtree::Node> nd;
        if (mHoveringNode != 0 && mHoveringNodeChild != -1 && ( nd = mHoveringNode->getNode()).get() != 0 && nd != mAddingNode) {
            mTree->connect(mAddingNode, nd, mHoveringNodeChild);
            mAddingItem->connectAsChild(mHoveringNode, mHoveringNodeChild);
            mAddingItem->update();
            emit nodeAdded(mAddingItem);
            mAddingNode.reset();
            mAddingItem = 0;

            endMode();
            return;
        }
        break;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void DtGraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
{
    switch (keyEvent->key()) {
    case Qt::Key_Escape:
        if (mMode == AddNode || mMode == AddNodeConnect) {
            // Abort add nodes
            mAddingNode.reset();
            if (mAddingItem)
                delete mAddingItem;
            mAddingItem = 0;
            endMode();
        }
        break;
    }
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
