// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "populationentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
#include <displacemodel.h>

// ENABLE sub-trees by uncommenting the following line

//#define ENABLE_POP_SZGROUPS

namespace objecttree {

PopulationEntity::PopulationEntity(ObjectTreeModel *_model, int idx)
    : ObjectTreeEntity(_model),
      mPopulationIndex(idx)
{
}

PopulationEntity::~PopulationEntity()
{

}

QModelIndex PopulationEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Populations);
}

QModelIndex PopulationEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity;
    if (mPopulationIndex == -1) {
        entity = new PopulationEntity(model, row);
    } else {
#ifdef ENABLE_POP_SZGROUPS
        entity = new PopulationEntityWithGroup(model, this, row);
#else
        return QModelIndex();
#endif
    }

    return model->createEntity(row, column, entity);
}

int PopulationEntity::rowCount() const
{
    if (model->getModel() != 0 && model->getModelIdx() != -1) {
        if (mPopulationIndex == -1)
            return model->getModel()->getPopulationsCount();
        else {
#ifdef ENABLE_POP_SZGROUPS
            return model->getModel()->getSzGrupsCount()+1;
#endif
        }
    }

    return 0;
}

int PopulationEntity::columnCount() const
{
    return 1;
}

QVariant PopulationEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString(QObject::tr("Population #%1")).arg(index.row());
    if (role == Qt::CheckStateRole)
        return QVariant(model->getModel()->isInterestingPop(index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags PopulationEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool PopulationEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingPop(index.row());
        } else {
            model->getModel()->setInterestingPop(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

QModelIndex PopulationEntity::makeIndex()
{
    return model->createEntity(mPopulationIndex, 0, this);
}

PopulationEntityWithGroup::PopulationEntityWithGroup(ObjectTreeModel *_model, const PopulationEntity *parent, int idx)
    : ObjectTreeEntity(_model),
      mParent(const_cast<PopulationEntity *>(parent)),
      mGroupIndex(idx)
{
}

PopulationEntityWithGroup::~PopulationEntityWithGroup()
{
}

QModelIndex PopulationEntityWithGroup::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mParent->makeIndex();
}

QModelIndex PopulationEntityWithGroup::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    Q_UNUSED(parent);
    return QModelIndex();
}

int PopulationEntityWithGroup::rowCount() const
{
    return 0;
}

int PopulationEntityWithGroup::columnCount() const
{
    return 1;
}

QVariant PopulationEntityWithGroup::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.row() == 0) {
            return QString(QObject::tr("Total"));
        }
        return QString(QObject::tr("Size Group #%1")).arg(index.row()-1);
    }
    if (role == Qt::CheckStateRole)
        return Qt::Unchecked;
//        return QVariant(model->getModel()->isInterestingPop(index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags PopulationEntityWithGroup::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defflags | Qt::ItemIsUserCheckable;
}

bool PopulationEntityWithGroup::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}


}
