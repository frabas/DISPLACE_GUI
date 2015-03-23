#include "graphnodeitem.h"
#include <dtree/node.h>

double GraphNodeItem::sDefWidth = 100.;
double GraphNodeItem::sDefHeight = 60.;

GraphNodeItem::GraphNodeItem(boost::shared_ptr<dtree::Node> node, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent),
      mNode(node)
{
    mRect = new QGraphicsRectItem();
    mRect->setRect(-sDefWidth/2, -sDefHeight/2, sDefWidth, sDefHeight - sDefHeight/3);
    addToGroup(mRect);

    for (int i = 0; i < node->getChildrenCount(); ++i) {
        QRectF r( -sDefWidth/2 + i*sDefWidth/node->getChildrenCount(),
                  sDefHeight/2 - sDefHeight/3,
                  sDefWidth / node->getChildrenCount(),
                  sDefHeight/3);

        QGraphicsRectItem *newch = new QGraphicsRectItem(r);
        mChildren.append(newch);

        addToGroup(newch);
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
