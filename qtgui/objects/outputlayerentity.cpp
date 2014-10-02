#include "outputlayerentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

OutputLayerEntity::OutputLayerEntity(ObjectTreeModel *_model, int idx)
    : ObjectTreeEntity(_model),
      mOutputLayerEntityIndex(idx)
{

}

OutputLayerEntity::~OutputLayerEntity()
{

}

QModelIndex OutputLayerEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::OutputLayers);
}

QModelIndex OutputLayerEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new OutputLayerEntity(model, row);
    return model->createEntity(row, column, entity);
}

int OutputLayerEntity::rowCount() const
{
    if (mOutputLayerEntityIndex == -1 && model->getModelIdx() != -1)
        return model->getMapControl()->getOutputLayerList(model->getModelIdx())->getCount();

    return 0;
}

int OutputLayerEntity::columnCount() const
{
    return 1;
}

QVariant OutputLayerEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return model->getMapControl()->getOutputLayerList(model->getModelIdx())->getName(index.row()); //->getLayers().at(index.row())->getName());
    if (role == Qt::CheckStateRole)
        return QVariant(model->getMapControl()->isOutLayerVisible(model->getModelIdx(), (MapObjectsController::OutLayerIds) index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags OutputLayerEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool OutputLayerEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getMapControl()->setOutLayerVisibility(model->getModelIdx(), (MapObjectsController::OutLayerIds)index.row(), false);
        } else {
            model->getMapControl()->setOutLayerVisibility(model->getModelIdx(), (MapObjectsController::OutLayerIds)index.row(), true);
        }
        return true;
    }
    return false;

}

}
