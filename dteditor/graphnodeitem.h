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

    // These mimics the Node's counterparts.
    int getChildrenCount() const;
    GraphNodeItem *getChild(int idx);
    void setChild(int idx, GraphNodeItem *child);
    void setParent(GraphNodeItem *child);
    int getChildrenId() const { return mChildrenId; }

    void connectAsParent(GraphNodeItem *item, int idx);
    void connectAsChild(GraphNodeItem *item, int idx);

    void moveArrow(QPointF pt);

    bool requiresChildrenHighlight() const;
    void childHoverEntered(int id);
    void childHoverExited();

    void update();

protected:
    void createArrow();
    QPointF getChildrenArrowLocation(int idx) const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    DtGraphicsScene *mScene;
    GraphNodeItem *mParent;
    QVector<GraphNodeItem *> mChildrenItems;

    boost::weak_ptr<dtree::Node> mNode;

    QGraphicsRectItem *mRect;
    QGraphicsTextItem *mText;
    int mChildrenId;
    QVector<GraphNodeChildBoxItem *> mChildrenBoxes;
    QGraphicsLineItem *mArrow;

    int mHoveredChild;

    static double sDefWidth, sDefHeight;
};

class GraphNodeChildBoxItem : public QGraphicsRectItem
{
public:
    explicit GraphNodeChildBoxItem(QRectF r, GraphNodeItem *parent, int id);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    GraphNodeItem *mParent;
    int mId;

    static QBrush mNormalBrush, mHighlightBrush;
};

#endif // GRAPHNODEITEM_H
