// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "graphnodeitem.h"
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

#include <QGraphicsScene>
#include <QDebug>

double GraphNodeItem::sDefWidth = 100.;
double GraphNodeItem::sDefHeight = 60.;

GraphNodeItem::GraphNodeItem(std::shared_ptr<dtree::Node> node, DtGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent),
      mScene(scene),
      mParent(0),
      mChildrenItems(),
      mNode(node),
      mChildrenId(-1),
      mArrow(0),
      mHoveredChild(-1)
{
    mRect = new QGraphicsRectItem();
    mRect->setRect(-sDefWidth/2, -sDefHeight/2, sDefWidth, sDefHeight - sDefHeight/3);
    addToGroup(mRect);

    mText = new QGraphicsTextItem(mRect);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setHandlesChildEvents(false);
}

GraphNodeItem::~GraphNodeItem()
{
    if (mRect)
        delete mRect;
    /* no: mText is children of mRect
    if (mText)
        delete mText;*/
    if (mArrow)
        delete mArrow;

    foreach (GraphNodeChildBoxItem *item, mChildrenBoxes) {
        if (item)
            delete item;
    }

    /* no mChildrenBoxText: is child of mChildrenBox
    foreach(QGraphicsTextItem *item, mChildrenBoxText) {
        if (item)
            delete item;
    }*/
}

int GraphNodeItem::getChildrenCount() const
{
    return mChildrenItems.size();
}

GraphNodeItem *GraphNodeItem::getChild(int idx)
{
    return mChildrenItems[idx];
}

void GraphNodeItem::setVariable(dtree::Variable var)
{
    Q_UNUSED(var);

    std::shared_ptr<dtree::Node> node = getNode();

    QVector<int> nummaps(node->getChildrenCount(), 0);

    int mx = -1;
    int n = node->getChildrenCount();
    for (int i = 0; i < n; ++i) {
        int m = node->getMapping(i);
        mx = std::max(mx, m);
        ++nummaps[m];
    }

    n = mx+1;
    QVector<GraphNodeItem *> v = mChildrenItems;
    for (int i = n; i < v.size(); ++i) {
        delete v[i];
    }

    mChildrenItems.clear();
    foreach (GraphNodeChildBoxItem *item, mChildrenBoxes)
        delete item;
    mChildrenBoxes.clear();

    mChildrenBoxText.clear(); // are children of mChilderBoxes.

    int twidth = 0;
    for (int i = 0; i < n; ++i) {
        if (i < v.size())
            mChildrenItems.push_back(v[i]);
        else
            mChildrenItems.push_back(0);

        QRectF r( -sDefWidth/2 + twidth,
                  sDefHeight/2 - sDefHeight/3,
                  sDefWidth / n,
                  sDefHeight/3);

        r = mapRectToScene(r);
        GraphNodeChildBoxItem *newch = new GraphNodeChildBoxItem(r, this, i);
        mChildrenBoxes.append(newch);
        QGraphicsTextItem *ti = new QGraphicsTextItem(newch);
        ti->setPos(r.topLeft());
        mChildrenBoxText.push_back(ti);

        QString label;
        if (nummaps[i] > 1) {
            label = QString(QObject::tr("%1")).arg(i);
        } else {
            label = QString::fromLatin1(dtree::VariableNames::variableBin(var, i));
        }
        ti->setPlainText(label);

        addToGroup(newch);

        r.setWidth(ti->boundingRect().width());
        newch->setRect(r);
        twidth += ti->boundingRect().width();
    }

    if (twidth > 0) {
        QRectF r= mRect->rect();
        r.setWidth(twidth);
        mRect->setRect(r);
    }
}

void GraphNodeItem::setChild(int idx, GraphNodeItem *child)
{
    mChildrenItems[idx] = child;
}

void GraphNodeItem::setParent(GraphNodeItem *child)
{
    mParent = child;
}

void GraphNodeItem::connectAsParent(GraphNodeItem *item, int idx)
{
    setChild(idx, item);
    item->setParent(this);
    item->mChildrenId = idx;
}

void GraphNodeItem::connectAsChild(GraphNodeItem *item, int idx)
{
    setParent(item);
    item->setChild(idx, this);
    mChildrenId = idx;
}

void GraphNodeItem::unlinkParent()
{
    mParent = 0;
}

void GraphNodeItem::unlinkChild(int idx)
{
    mChildrenItems[idx] = 0;
}

void GraphNodeItem::moveArrow(QPointF pt)
{
    if (mArrow == 0) {
        createArrow();
    }

    QPointF p1 = mapToScene(0, -sDefHeight/2);

    QLineF line (p1, pt);
    mArrow->setLine(line);
    mArrow->setVisible(true);
}

bool GraphNodeItem::requiresChildrenHighlight(int childid) const
{
    return mScene->requiresChildrenHighlight() && mChildrenItems[childid] == 0;
}

void GraphNodeItem::childHoverEntered(int id)
{
    mScene->nodeChildEntered(this, id);
}

void GraphNodeItem::childHoverExited()
{
    mScene->nodeChildExited();
}

void GraphNodeItem::update()
{
    if (mArrow == 0)
        createArrow();

    std::shared_ptr<dtree::Node> node = getNode();
    if (node) {
        switch (node->variable()) {
        case dtree::Variable::VarUndefined:
            mText->setPlainText(QString());
            break;
        case dtree::Variable::VarLeaf:
            mText->setPlainText(QString("%1").arg(node->value(),0,'f',3));
            break;
        default:
            mText->setPlainText(dtree::VariableNames::variableName(node->variable()));
        }
    }

    if (getChildrenId() != -1 && mParent) {
        QPointF p1 = mapToScene(0, -sDefHeight/2);
        QPointF p2 = mParent->getChildrenArrowLocation(getChildrenId());

        QLineF line (p1, p2);
        mArrow->setLine(line);
        mArrow->setVisible(true);
    } else {
        mArrow->setVisible(false);
    }

    double r = mText->textWidth();
    QRectF p = mRect->rect();
    mText->setPos(p.left(), p.top());
}

void GraphNodeItem::createArrow()
{
    mArrow = new QGraphicsLineItem;
    mScene->addItem(mArrow);
    mArrow->setVisible(false);
}

QPointF GraphNodeItem::getChildrenArrowLocation(int idx) const
{
    return mChildrenBoxes[idx]->mapToScene(mChildrenBoxes[idx]->rect().center().x(), mChildrenBoxes[idx]->rect().bottom());
}

QVariant GraphNodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange:
        foreach (GraphNodeItem*item, mChildrenItems) {
            if (item)
                item->update();
        }
        update();
        break;
    default:
        break;
    }

    return QGraphicsItemGroup::itemChange(change, value);
}

QBrush GraphNodeChildBoxItem::mNormalBrush(Qt::transparent);
QBrush GraphNodeChildBoxItem::mHighlightBrush(Qt::red);

GraphNodeChildBoxItem::GraphNodeChildBoxItem(QRectF r, GraphNodeItem *parent, int id)
    : QGraphicsRectItem(r),
      mParent(parent),
      mId(id)
{
    setAcceptHoverEvents(true);
}

void GraphNodeChildBoxItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (mParent->requiresChildrenHighlight(mId)) {
        setBrush(mHighlightBrush);
        mParent->childHoverEntered(mId);
    }
    QGraphicsRectItem::hoverEnterEvent(event);
}

void GraphNodeChildBoxItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(mNormalBrush);
    mParent->childHoverExited();
    QGraphicsRectItem::hoverLeaveEvent(event);
}
