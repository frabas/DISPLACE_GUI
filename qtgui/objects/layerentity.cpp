// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <objects/layerentity.h>

#include <displacemodel.h>
#include <objecttreemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {


LayerEntity::LayerEntity(ObjectTreeModel::Category type, ObjectTreeModel *_model, int LayerEntity_idx)
    : ObjectTreeEntity(_model),
      mLayerEntityIndex(LayerEntity_idx),
      mLayerEntityType(type)
{
}

LayerEntity::~LayerEntity()
{

}

QModelIndex LayerEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(mLayerEntityType);
}

QModelIndex LayerEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new LayerEntity(mLayerEntityType, model, row);
    return model->createEntity(row, column, entity);
}

int LayerEntity::rowCount() const
{
    if (mLayerEntityIndex == -1 && model->getModelIdx() != -1)
        return model->getMapControl()->getLayerList(model->getModelIdx(), mLayerEntityType)->getCount();

    return 0;
}

int LayerEntity::columnCount() const
{
    return 1;
}

QVariant LayerEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QString name = model->getMapControl()->getLayerList(model->getModelIdx(), mLayerEntityType)->getName(index.row());
        if (name.startsWith('#')) {
            // Layer name has control index: clean the name.
            name = name.mid(name.lastIndexOf('#')+1);
        }
        return name;
    }
    if (role == Qt::CheckStateRole)
        return QVariant(model->getMapControl()->isLayerVisible(model->getModelIdx(), mLayerEntityType, (MapObjectsController::LayerIds) index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags LayerEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool LayerEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getMapControl()->setLayerVisibility(model->getModelIdx(), mLayerEntityType, (MapObjectsController::LayerIds)index.row(), false);
        } else {
            model->getMapControl()->setLayerVisibility(model->getModelIdx(), mLayerEntityType, (MapObjectsController::LayerIds)index.row(), true);
        }
        return true;
    }
    return false;
}

}
