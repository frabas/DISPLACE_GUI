#ifndef DTGRAPHICSSCENE_H
#define DTGRAPHICSSCENE_H

#include <QGraphicsScene>

#include <boost/shared_ptr.hpp>
#include <set>

namespace dtree {
class DecisionTree;
class GraphNodeExtra;
class Node;
}

class GraphNodeItem;

class DtGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DtGraphicsScene(boost::shared_ptr<dtree::DecisionTree> tree, QObject *parent = 0);

    void clear();
    bool requiresChildrenHighlight() const;

    void nodeChildEntered (GraphNodeItem *item, int childId);
    void nodeChildExited();

    GraphNodeItem *root() const { return mRoot; }
    void addItemAsRoot(GraphNodeItem *item);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void nodeAdded(GraphNodeItem *);
    void mouseModeEnded();

public slots:
    void startAddNode(boost::shared_ptr<dtree::Node>);
    void endMode();

private:
    enum Mode {
        None, AddNode, AddNodeConnect
    };

    Mode mMode = None;

    boost::shared_ptr<dtree::DecisionTree> mTree;
    GraphNodeItem *mRoot;

    //////
    boost::shared_ptr<dtree::Node> mAddingNode;
    GraphNodeItem *mAddingItem;

    GraphNodeItem *mHoveringNode;
    int mHoveringNodeChild;
};

#endif // DTGRAPHICSSCENE_H
