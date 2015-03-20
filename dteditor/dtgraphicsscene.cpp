#include "dtgraphicsscene.h"
#include <QGraphicsSceneMouseEvent>
#include <graphnodeitem.h>

DtGraphicsScene::DtGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

void DtGraphicsScene::addUnconnectedNode(boost::shared_ptr<dtree::Node> node, QPointF pos)
{
    GraphNodeItem *item = new GraphNodeItem(node);
    item->setPos(pos);

    mUnconnectedNodes.insert(node);
    addItem(item);
}

void DtGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (mMode) {
    case None:
        break;
    case AddNode:
        endMode();
        emit nodeAddCompleted(event->scenePos());
        break;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void DtGraphicsScene::startAddNode()
{
    mMode = AddNode;
}

void DtGraphicsScene::endMode()
{
    mMode = None;
    emit mouseModeEnded();
}
