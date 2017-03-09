// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "benthosentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

BenthosEntity::BenthosEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model), mId(id)
{
}

QModelIndex BenthosEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Benthos);
}

QModelIndex BenthosEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new BenthosEntity(model, row);
    return model->createEntity(row, column, entity);
}

int BenthosEntity::rowCount() const
{
    if (mId == -1 && model->getModel() != 0)
        return model->getModel()->getBenthosCount();

    return 0;
}

int BenthosEntity::columnCount() const
{
    return 1;
}

QVariant BenthosEntity::data(const QModelIndex &index, int role) const
{
    if (mId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return QString("%1").arg(model->getModel()->getBenthosList()[mId]->getId());
        // Disable the tickmarks, we don't need them... yet.
        if (role == Qt::CheckStateRole)
            return QVariant(model->getModel()->isInterestingBenthos(index.row()) ? Qt::Checked : Qt::Unchecked);
        if (role == Qt::TextColorRole)
            return model->getMapControl()->getPalette(model->getModelIdx(), PopulationRole).color(mId);

        /*
        if (role == Qt::ToolTipRole) {
            Vessel *v = model->getModel()->getVesselList()[mVesselId];
            return QString("%1 %2").arg(v->get_y()).arg(v->get_x());
        }*/
    }

    return QVariant();
}

}


Qt::ItemFlags objecttree::BenthosEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool objecttree::BenthosEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingBenthos(index.row());
        } else {
            model->getModel()->setInterestingBenthos(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}
