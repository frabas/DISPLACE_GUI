#include "graphnodeitem.h"

double GraphNodeItem::sDefWidth = 100.;
double GraphNodeItem::sDefHeight = 60.;

GraphNodeItem::GraphNodeItem(boost::shared_ptr<dtree::Node> node, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent),
      mNode(node)
{
    mPoly << QPointF(-sDefWidth/2, -sDefHeight/2) << QPointF(sDefWidth/2, -sDefHeight/2)
          << QPointF(sDefWidth/2, sDefHeight/2) << QPointF(-sDefWidth/2, sDefHeight/2)
          << QPointF(-sDefWidth/2, -sDefHeight/2);

    setPolygon(mPoly);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
