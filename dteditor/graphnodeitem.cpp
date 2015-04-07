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

void GraphNodeItem::setVariable(dtree::Variable var)
{
    Q_UNUSED(var);

    boost::shared_ptr<dtree::Node> node = getNode();
    int n = node->getChildrenCount();

    if (n != mChildrenItems.size()) {
        QVector<GraphNodeItem *> v = mChildrenItems;
        mChildrenItems.clear();
        foreach (GraphNodeChildBoxItem *item, mChildrenBoxes)
            delete item;

        mChildrenBoxes.clear();
        for (int i = 0; i < n; ++i) {
            if (i < v.size())
                mChildrenItems.push_back(v[i]);
            else
                mChildrenItems.push_back(0);

            QRectF r( -sDefWidth/2 + i*sDefWidth/n,
                      sDefHeight/2 - sDefHeight/3,
                      sDefWidth / n,
                      sDefHeight/3);

            GraphNodeChildBoxItem *newch = new GraphNodeChildBoxItem(mapRectToScene(r), this, i);
            mChildrenBoxes.append(newch);

            addToGroup(newch);
        }
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

    boost::shared_ptr<dtree::Node> node = getNode();
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
    mText->setPos(-(p.width() - r) / 2, p.top());
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
