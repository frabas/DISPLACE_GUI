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

#include "nationentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
#include <displacemodel.h>

namespace objecttree {

NationEntity::NationEntity(ObjectTreeModel *_model, int idx)
    : ObjectTreeEntity(_model),
      mNationIndex(idx)
{
}

NationEntity::~NationEntity()
{

}

QModelIndex NationEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Nations);
}

QModelIndex NationEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new NationEntity(model, row);
    return model->createEntity(row, column, entity);
}

int NationEntity::rowCount() const
{
    if (mNationIndex == -1 && model->getModel() != 0 && model->getModelIdx() != -1)
        return model->getModel()->getNationsList().size();

    return 0;
}

int NationEntity::columnCount() const
{
    return 1;
}

QVariant NationEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString(model->getModel()->getNation(index.row()).getName());
    if (role == Qt::CheckStateRole)
        return QVariant(model->getModel()->isInterestingNations(index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags NationEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool NationEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingNations(index.row());
        } else {
            model->getModel()->setInterestingNations(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

}
