#ifndef GRAPHNODEITEM_H
#define GRAPHNODEITEM_H

#include <QGraphicsItemGroup>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace dtree {
    class Node;
}

class GraphNodeItem : public QGraphicsItemGroup
{
public:
    explicit GraphNodeItem(boost::shared_ptr<dtree::Node> node, QGraphicsItem *parent = 0);

    boost::shared_ptr<dtree::Node> getNode() const { return mNode.lock(); }

signals:

public slots:

private:
    boost::weak_ptr<dtree::Node> mNode;
    QGraphicsRectItem *mRect;
    QVector<QGraphicsRectItem *> mChildren;

    static double sDefWidth, sDefHeight;
};

#endif // GRAPHNODEITEM_H
