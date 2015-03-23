#ifndef GRAPHNODEITEM_H
#define GRAPHNODEITEM_H

#include <QGraphicsItemGroup>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace dtree {
    class Node;
}

class DtGraphicsScene;
class GraphNodeChildBoxItem;

class GraphNodeItem : public QGraphicsItemGroup
{
public:
    explicit GraphNodeItem(boost::shared_ptr<dtree::Node> node, DtGraphicsScene *scene, QGraphicsItem *parent = 0);

    boost::shared_ptr<dtree::Node> getNode() const { return mNode.lock(); }

    void moveArrow(QPointF pt);

private:
    DtGraphicsScene *mScene;
    boost::weak_ptr<dtree::Node> mNode;
    QGraphicsRectItem *mRect;
    QVector<GraphNodeChildBoxItem *> mChildren;
    QGraphicsLineItem *mArrow;

    static double sDefWidth, sDefHeight;
};

class GraphNodeChildBoxItem : public QGraphicsRectItem
{
public:
    explicit GraphNodeChildBoxItem(QRectF r, DtGraphicsScene *scene);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    DtGraphicsScene *mScene;

    static QBrush mNormalBrush, mHighlightBrush;
};

#endif // GRAPHNODEITEM_H
