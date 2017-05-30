// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "fishfarmentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

FishfarmEntity::FishfarmEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mFishfarmId(id)
{
}

QModelIndex FishfarmEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Fishfarms);
}

QModelIndex FishfarmEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new FishfarmEntity(model, row);
  //   std::cout << "id (row) is assigned here and is " << row << endl;
    return model->createEntity(row, column, entity);
}

int FishfarmEntity::rowCount() const
{
    if (mFishfarmId == -1 && model->getModel() != 0)
        return model->getModel()->getFishfarmsCount();

    return 0;
}

int FishfarmEntity::columnCount() const
{
    return 1;
}

QVariant FishfarmEntity::data(const QModelIndex &index, int role) const
{
    if (mFishfarmId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
              return QString("%1").arg(model->getModel()->getFishfarmId(mFishfarmId));
        if (role == Qt::CheckStateRole)
            return QVariant(model->getModel()->isInterestingFishfarms(index.row()) ? Qt::Checked : Qt::Unchecked);
        if (role == Qt::ToolTipRole) {
            std::shared_ptr<FishfarmData> ff = model->getModel()->getFishfarmList()[mFishfarmId];
            return QString("%1 %2").arg(ff->mFishfarm->get_y()).arg(ff->mFishfarm->get_x());
        }


     model->getModel()->setInterestingFishfarms(index.row());
    }

    return QVariant();
}

}


Qt::ItemFlags objecttree::FishfarmEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool objecttree::FishfarmEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingFishfarms(index.row());
        } else {
            model->getModel()->setInterestingFishfarms(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        //model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

