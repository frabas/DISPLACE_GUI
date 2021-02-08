// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "shipentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

ShipEntity::ShipEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mShipId(id)
{
}

QModelIndex ShipEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Ships);
}

QModelIndex ShipEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new ShipEntity(model, row);
 // std::cout << "id (row) is assigned here and is " << row << endl;
    return model->createEntity(row, column, entity);
}

int ShipEntity::rowCount() const
{
    if (mShipId == -1 && model->getModel() != 0)
        return model->getModel()->getShipsCount();

    return 0;
}

int ShipEntity::columnCount() const
{
    return 1;
}

QVariant ShipEntity::data(const QModelIndex &index, int role) const
{
    if (mShipId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return model->getModel()->getShipId(mShipId);
        if (role == Qt::CheckStateRole)
            return QVariant(model->getModel()->isInterestingShips(index.row()+1) ? Qt::Checked : Qt::Unchecked);
        if (role == Qt::ToolTipRole) {
            std::shared_ptr<ShipData> sh = model->getModel()->getShipList()[mShipId];
            return QString("%1 %2").arg(sh->mShip->get_y()).arg(sh->mShip->get_x());
        }
    }

    return QVariant();
}


Qt::ItemFlags objecttree::ShipEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

// Note WindFarm ids are 1-based
bool objecttree::ShipEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingShips(index.row()+1);
        } else {
            model->getModel()->setInterestingShips(index.row()+1);
        }
        model->getStatsController()->updateStats(model->getModel());
        //model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}


}
