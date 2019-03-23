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

#include "szgroupentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
#include <displacemodel.h>

// ENABLE sub-trees by uncommenting the following line

//#define ENABLE_POP_SZGROUPS

namespace objecttree {

SzGroupEntity::SzGroupEntity(ObjectTreeModel *_model, int idx)
    : ObjectTreeEntity(_model),
      mGroupIndex(idx)
{
}

SzGroupEntity::~SzGroupEntity()
{
}


QModelIndex SzGroupEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::SizeGroups);
}

QModelIndex SzGroupEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = 0;
    if (mGroupIndex == -1) {
        entity = new SzGroupEntity(model, row);
    }

    return model->createEntity(row, column, entity);
}

int SzGroupEntity::rowCount() const
{
    if (model->getModel() != 0 && model->getModelIdx() != -1) {
        if (mGroupIndex == -1)
            return model->getModel()->getSzGrupsCount() + 4;    // Total, Average
    }

    return 0;
}

int SzGroupEntity::columnCount() const
{
    return 1;
}

QVariant SzGroupEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.row()) {
        case Total:
            return QString(QObject::tr("Total"));
        case Average:
            return QString(QObject::tr("Average"));
        case Min:
            return QString(QObject::tr("Min"));
        case Max:
            return QString(QObject::tr("Max"));
        default:
            return QString(QObject::tr("Size/Age Group #%1")).arg(index.row() - LastSpecialGroup +1);
        }
    }

    if (role == Qt::CheckStateRole) {
        switch (index.row()) {
        case Total:
            return QVariant(model->getModel()->isInterestingSizeTotal() ? Qt::Checked : Qt::Unchecked);
        case Average:
            return QVariant(model->getModel()->isInterestingSizeAvg() ? Qt::Checked : Qt::Unchecked);
        case Min:
            return QVariant(model->getModel()->isInterestingSizeMin() ? Qt::Checked : Qt::Unchecked);
        case Max:
            return QVariant(model->getModel()->isInterestingSizeMax() ? Qt::Checked : Qt::Unchecked);
        default:
            return QVariant(model->getModel()->isInterestingSize(index.row() - LastSpecialGroup) ? Qt::Checked : Qt::Unchecked);
        }
    }

    return QVariant();
}

Qt::ItemFlags SzGroupEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool SzGroupEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {

        switch (index.row()) {
        case Total:
            model->getModel()->setInterestingSizeTotal(value.toInt() != 0);
            break;
        case Average:
            model->getModel()->setInterestingSizeAvg(value.toInt() != 0);
            break;
        case Min:
            model->getModel()->setInterestingSizeMin(value.toInt() != 0);
            break;
        case Max:
            model->getModel()->setInterestingSizeMax(value.toInt() != 0);
            break;
        default:
            if (value.toInt() == 0) {
                model->getModel()->remInterestingSize(index.row() - LastSpecialGroup);
            } else {
                model->getModel()->setInterestingSize(index.row() - LastSpecialGroup);
            }
            break;
        }

        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

}
