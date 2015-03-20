#ifndef GRAPHNODEITEM_H
#define GRAPHNODEITEM_H

#include <QGraphicsPolygonItem>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace dtree {
    class Node;
}

class GraphNodeItem : public QGraphicsPolygonItem
{
public:
    explicit GraphNodeItem(boost::shared_ptr<dtree::Node> node, QGraphicsItem *parent = 0);

    boost::shared_ptr<dtree::Node> getNode() const { return mNode.lock(); }

signals:

public slots:

private:
    boost::weak_ptr<dtree::Node> mNode;
    QPolygonF mPoly;
};

#endif // GRAPHNODEITEM_H
