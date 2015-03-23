#include "graphnodeitem.h"
#include <dtree/node.h>
#include <dtgraphicsscene.h>

#include <QGraphicsScene>
#include <QDebug>

double GraphNodeItem::sDefWidth = 100.;
double GraphNodeItem::sDefHeight = 60.;

GraphNodeItem::GraphNodeItem(boost::shared_ptr<dtree::Node> node, DtGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent),
      mScene(scene),
      mNode(node),
      mArrow(0)
{
    mRect = new QGraphicsRectItem();
    mRect->setRect(-sDefWidth/2, -sDefHeight/2, sDefWidth, sDefHeight - sDefHeight/3);
    addToGroup(mRect);

    for (int i = 0; i < node->getChildrenCount(); ++i) {
        QRectF r( -sDefWidth/2 + i*sDefWidth/node->getChildrenCount(),
                  sDefHeight/2 - sDefHeight/3,
                  sDefWidth / node->getChildrenCount(),
                  sDefHeight/3);

        GraphNodeChildBoxItem *newch = new GraphNodeChildBoxItem(r, mScene);
        mChildren.append(newch);

        addToGroup(newch);
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setHandlesChildEvents(false);
}

void GraphNodeItem::moveArrow(QPointF pt)
{
    if (mArrow == 0) {
        mArrow = new QGraphicsLineItem;
        mArrow->setVisible(false);
        scene()->addItem(mArrow);
    }

    QPointF p1 = mapToScene(0, -sDefHeight/2);

    QLineF line (p1, pt);
    mArrow->setLine(line);
    mArrow->setVisible(true);
}

QBrush GraphNodeChildBoxItem::mNormalBrush(Qt::transparent);
QBrush GraphNodeChildBoxItem::mHighlightBrush(Qt::red);

GraphNodeChildBoxItem::GraphNodeChildBoxItem(QRectF r, DtGraphicsScene *scene)
    : QGraphicsRectItem(r),
      mScene(scene)
{
    setAcceptHoverEvents(true);
}

void GraphNodeChildBoxItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (mScene->requiresChildrenHighlight()) {
        setBrush(mHighlightBrush);
    }
    QGraphicsRectItem::hoverEnterEvent(event);
}

void GraphNodeChildBoxItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(mNormalBrush);
    QGraphicsRectItem::hoverLeaveEvent(event);
}
