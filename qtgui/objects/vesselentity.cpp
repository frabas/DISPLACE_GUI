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

#include "vesselentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

VesselEntity::VesselEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mVesselId(id)
{
}

QModelIndex VesselEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Vessels);
}

QModelIndex VesselEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new VesselEntity(model, row);
 // std::cout << "id (row) is assigned here and is " << row << endl;
    return model->createEntity(row, column, entity);
}

int VesselEntity::rowCount() const
{
    if (mVesselId == -1 && model->getModel() != 0)
        return model->getModel()->getVesselCount();

    return 0;
}

int VesselEntity::columnCount() const
{
    return 1;
}

QVariant VesselEntity::data(const QModelIndex &index, int role) const
{
    if (mVesselId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return model->getModel()->getVesselId(mVesselId);
        if (role == Qt::ToolTipRole) {
            std::shared_ptr<VesselData> v = model->getModel()->getVesselList()[mVesselId];
            return QString("%1 %2").arg(v->mVessel->get_y()).arg(v->mVessel->get_x());
        }
        if (role == Qt::CheckStateRole)
            return QVariant(model->getModel()->isInterestingVessels(index.row()) ? Qt::Checked : Qt::Unchecked);

    }

    return QVariant();
}



Qt::ItemFlags VesselEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool VesselEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingVessels(index.row());
        } else {
            model->getModel()->setInterestingVessels(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}



}
