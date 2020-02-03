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

#include "harbourentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
#include <displacemodel.h>

namespace objecttree {

HarbourEntity::HarbourEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mHarbourId(id)
{
}

QModelIndex HarbourEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Harbours);
}

QModelIndex HarbourEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new HarbourEntity(model, row);
    return model->createEntity(row, column, entity);
}

int HarbourEntity::rowCount() const
{
    if (mHarbourId == -1 && model->getModel() != 0)
        return model->getModel()->getHarboursCount();

    return 0;
}

int HarbourEntity::columnCount() const
{
    return 1;
}

QVariant HarbourEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return model->getModel()->getHarbourId(mHarbourId);
    if (role == Qt::CheckStateRole)
        return QVariant(model->getModel()->isInterestingHarb(types::NodeId(index.row())) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags HarbourEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defflags | Qt::ItemIsUserCheckable;
}

bool HarbourEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingHarb(types::NodeId(index.row()));
        } else {
            model->getModel()->setInterestingHarb(types::NodeId(index.row()));
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

}
