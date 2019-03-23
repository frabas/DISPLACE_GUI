// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef DTGRAPHICSSCENE_H
#define DTGRAPHICSSCENE_H

#include <QGraphicsScene>

#include <memory>
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
    explicit DtGraphicsScene(std::shared_ptr<dtree::DecisionTree> tree, QObject *parent = 0);

    void clear();
    bool requiresChildrenHighlight() const;

    void nodeChildEntered (GraphNodeItem *item, int childId);
    void nodeChildExited();

    GraphNodeItem *root() const { return mRoot; }
    void addItemAsRoot(GraphNodeItem *item);

    void removeNodes (QList<GraphNodeItem *>items);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent * keyEvent) override;

signals:
    void nodeAdded(GraphNodeItem *);
    void mouseModeEnded();

public slots:
    void startAddNode(std::shared_ptr<dtree::Node>);
    void endMode();

private:
    enum Mode {
        None, AddNode, AddNodeConnect
    };

    Mode mMode = None;

    std::shared_ptr<dtree::DecisionTree> mTree;
    GraphNodeItem *mRoot;

    //////
    std::shared_ptr<dtree::Node> mAddingNode;
    GraphNodeItem *mAddingItem;

    GraphNodeItem *mHoveringNode;
    int mHoveringNodeChild;
};

#endif // DTGRAPHICSSCENE_H
