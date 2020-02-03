// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef GRAPHNODEITEM_H
#define GRAPHNODEITEM_H

#include <QGraphicsItemGroup>
#include <memory>
#include <boost/weak_ptr.hpp>
#include <dtree/variables.h>

namespace dtree {
    class Node;
}

class DtGraphicsScene;
class GraphNodeChildBoxItem;

class GraphNodeItem : public QGraphicsItemGroup
{
public:
    explicit GraphNodeItem(std::shared_ptr<dtree::Node> node, DtGraphicsScene *scene, QGraphicsItem *parent = 0);
    virtual ~GraphNodeItem();

    std::shared_ptr<dtree::Node> getNode() const { return mNode.lock(); }

    // These mimics the Node's counterparts.
    int getChildrenCount() const;
    GraphNodeItem *getChild(int idx);
    int getChildrenId() const { return mChildrenId; }
    GraphNodeItem *getParent() const { return mParent; }

    void setVariable(dtree::Variable var);

    void connectAsParent(GraphNodeItem *item, int idx);
    void connectAsChild(GraphNodeItem *item, int idx);
    void unlinkParent();
    void unlinkChild(int idx);

    void moveArrow(QPointF pt);

    bool requiresChildrenHighlight(int childid) const;
    void childHoverEntered(int id);
    void childHoverExited();

    void update();

protected:
    void setChild(int idx, GraphNodeItem *child);
    void setParent(GraphNodeItem *child);

    void createArrow();
    QPointF getChildrenArrowLocation(int idx) const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    DtGraphicsScene *mScene;
    GraphNodeItem *mParent;
    QVector<GraphNodeItem *> mChildrenItems;

    std::weak_ptr<dtree::Node> mNode;

    QGraphicsRectItem *mRect;
    QGraphicsTextItem *mText;
    int mChildrenId;
    QVector<GraphNodeChildBoxItem *> mChildrenBoxes;
    QVector<QGraphicsTextItem *> mChildrenBoxText;
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
