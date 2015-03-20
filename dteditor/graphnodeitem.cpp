#include "graphnodeitem.h"

GraphNodeItem::GraphNodeItem(boost::shared_ptr<dtree::Node> node, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent),
      mNode(node)
{
    mPoly << QPointF(-100, -100) << QPointF(100, -100)
          << QPointF(100, 100) << QPointF(-100, 100)
          << QPointF(-100, -100);

    setPolygon(mPoly);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
