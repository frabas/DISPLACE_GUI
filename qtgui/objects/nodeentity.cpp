// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "nodeentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <QMapControl/QMapControl.h>
#include <mapobjectscontroller.h>

#include <QInputDialog>

namespace objecttree {

NodeEntity::NodeEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mNodeId(id),
      mContextMenu(0)
{
}

QModelIndex NodeEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Nodes);
}

QModelIndex NodeEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (mNodeId == -1 && model->getModel() != 0 && mEntities.size() == 0) {
        // make a lazy initialization here.
        const_cast<NodeEntity*>(this)->init();
    }

    if (mEntities.size() > row) {
        return model->createEntity(row, column, mEntities.at(row));
    }
    else
        return model->createEntity(row, column, nullptr);
}

int NodeEntity::rowCount() const
{
    if (mNodeId == -1 && model->getModel() != 0)
        return model->getModel()->getNodesCount();

    return 0;
}

int NodeEntity::columnCount() const
{
    return 1;
}

QVariant NodeEntity::data(const QModelIndex &index, int role) const
{
    if (mNodeId != -1 && model->getModel() != 0 && role == Qt::DisplayRole && index.column() == 0) {
    //    return model->getModel()->getNodeId(mNodeId);

        std::shared_ptr<NodeData> n (model->getModel()->getNodesList()[mNodeId]);
                return n->get_idx_node().toIndex();
    }
    return QVariant();
}

Qt::ItemFlags NodeEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);

    return defflags;
}

bool NodeEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;   // item is not editable
}

QMenu *NodeEntity::contextMenu() const
{
    if (mContextMenu == 0) {
        const_cast<NodeEntity*>(this)->initMenu();
    }

    return mContextMenu;
}

void NodeEntity::onActionSearchById()
{
    bool ok;
    int id = QInputDialog::getInt(0,
                                  tr("Search node by ID"),
                                  tr("Please enter the node ID"),
                                  mNodeId, 0, model->getModel()->getNodesCount()-1,
                                  1, &ok);


    if (ok) {
        std::shared_ptr<NodeData> n (model->getModel()->getNodesList()[id]);

        model->getMapControl()->mapWidget()->setMapFocusPointAnimated(
                    qmapcontrol::PointWorldCoord(n->get_x(), n->get_y()),
                    5, std::chrono::milliseconds(100));
    }
}

void NodeEntity::init()
{
    for(int i = 0; i< model->getModel()->getNodesCount(); ++i) {
        mEntities.push_back(new NodeEntity(model, i));
    }
}

void NodeEntity::initMenu()
{
    mContextMenu = new QMenu();
    connect (mContextMenu->addAction(QObject::tr("Find node by Id...")), SIGNAL(triggered()), this, SLOT(onActionSearchById()));
}


}
