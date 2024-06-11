// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "dtgraphicsscene.h"
#include <dtree/decisiontree.h>

#include <QGraphicsSceneMouseEvent>
#include <QQueue>
#include <QKeyEvent>
#include <graphnodeitem.h>
#include <QDebug>

DtGraphicsScene::DtGraphicsScene(std::shared_ptr<dtree::DecisionTree> tree, QObject *parent) :
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
    if (mRoot) {
        QList<GraphNodeItem*> nodes;
        nodes.push_back(mRoot);
        removeNodes(nodes);
    }

    mRoot = 0;
    mAddingNode.reset();
    if (mAddingItem) delete mAddingItem;
    mAddingItem = 0;
    mHoveringNode = 0;
    mHoveringNodeChild = -1;
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

void DtGraphicsScene::removeNodes(QList<GraphNodeItem *> items)
{
    // Zero: collect unique objects
    QQueue<GraphNodeItem *> queue;
    queue.append(items);

    QSet<GraphNodeItem *> nodes_to_remove;
    while (!queue.empty()) {
        GraphNodeItem *node = queue.front();
        queue.pop_front();

        nodes_to_remove.insert(node);
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            GraphNodeItem *ch = node->getChild(i);
            if (ch) {
                queue.push_back(ch);
            }
        }
    }

    foreach (GraphNodeItem *node, nodes_to_remove) {
        // First, cut the parent
        if (node->getParent()) {
            node->getParent()->unlinkChild(node->getChildrenId());
            node->unlinkParent();
        } else {
            mRoot = 0;
        }
        // Second, cut the children
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            node->unlinkChild(i);
        }
    }

    foreach (GraphNodeItem *node, nodes_to_remove) {
        delete node;
    }
}

#if 0
void DtGraphicsScene::addUnconnectedNode(std::shared_ptr<dtree::Node> node, QPointF pos)
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
        std::shared_ptr<dtree::Node> nd;
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

void DtGraphicsScene::startAddNode(std::shared_ptr<dtree::Node> node)
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
