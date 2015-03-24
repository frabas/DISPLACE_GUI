#include "graphnodeitem.h"
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

#include <QGraphicsScene>
#include <QDebug>

double GraphNodeItem::sDefWidth = 100.;
double GraphNodeItem::sDefHeight = 60.;

GraphNodeItem::GraphNodeItem(boost::shared_ptr<dtree::Node> node, DtGraphicsScene *scene, QGraphicsItem *parent)
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

    for (int i = 0; i < node->getChildrenCount(); ++i) {
        QRectF r( -sDefWidth/2 + i*sDefWidth/node->getChildrenCount(),
                  sDefHeight/2 - sDefHeight/3,
                  sDefWidth / node->getChildrenCount(),
                  sDefHeight/3);

        GraphNodeChildBoxItem *newch = new GraphNodeChildBoxItem(r, this, i);
        mChildrenBoxes.append(newch);
        mChildrenItems.append(0);

        addToGroup(newch);
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setHandlesChildEvents(false);
}

int GraphNodeItem::getChildrenCount() const
{
    return mChildrenItems.size();
}

GraphNodeItem *GraphNodeItem::getChild(int idx)
{
    return mChildrenItems[idx];
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
}

void GraphNodeItem::connectAsChild(GraphNodeItem *item, int idx)
{
    setParent(item);
    item->setChild(idx, this);
    mChildrenId = idx;
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

bool GraphNodeItem::requiresChildrenHighlight() const
{
    return mScene->requiresChildrenHighlight();
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

    if (getChildrenId() != -1 && mParent) {
        QPointF p1 = mapToScene(0, -sDefHeight/2);
        QPointF p2 = mParent->mapToScene(mParent->getChildrenArrowLocation(getChildrenId()));

        QLineF line (p1, p2);
        mArrow->setLine(line);
        mArrow->setVisible(true);
    } else {
        mArrow->setVisible(false);
    }

    boost::shared_ptr<dtree::Node> node = getNode();
    if (node && node->variable() != dtree::Variable::VarUndefined) {
        mText->setPlainText(dtree::VariableNames::variableName(node->variable()));
    } else {
        mText->setPlainText("");
    }

    double r = mText->textWidth();
    QRectF p = mRect->rect();
    mText->setPos(-(p.width() - r) / 2, p.top());
}

void GraphNodeItem::createArrow()
{
    mArrow = new QGraphicsLineItem;
    scene()->addItem(mArrow);
    mArrow->setVisible(false);
}

QPointF GraphNodeItem::getChildrenArrowLocation(int idx) const
{
    return QPointF(mChildrenBoxes[idx]->rect().center().x(), mChildrenBoxes[idx]->rect().bottom());
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
    if (mParent->requiresChildrenHighlight()) {
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