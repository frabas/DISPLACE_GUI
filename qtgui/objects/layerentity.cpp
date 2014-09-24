#include <objects/LayerEntity.h>

#include <objecttreemodel.h>
#include <QMapControl/QMapControl.h>
#include <QMapControl/Layer.h>

namespace objecttree {

LayerEntity::LayerEntity(ObjectTreeModel *_model, int LayerEntity_idx)
    : ObjectTreeEntity(_model),
      mLayerEntityIndex(LayerEntity_idx)
{
}

LayerEntity::~LayerEntity()
{

}

QModelIndex LayerEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Layers);
}

QModelIndex LayerEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new LayerEntity(model, row);
    return model->createEntity(row, column, entity);
}

int LayerEntity::rowCount() const
{
    if (mLayerEntityIndex == -1)
        return model->getMapControl()->getLayers().size();

    return 0;
}

int LayerEntity::columnCount() const
{
    return 1;
}

QVariant LayerEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString::fromStdString(model->getMapControl()->getLayers().at(index.row())->getName());
    if (role == Qt::CheckStateRole)
        return QVariant(model->getMapControl()->getLayers().at(index.row())->isVisible() ? Qt::Checked : Qt::Unchecked);
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
            model->getMapControl()->getLayers().at(index.row())->setVisible(false);
        } else {
            model->getMapControl()->getLayers().at(index.row())->setVisible(true);
        }
        return true;
    }
    return false;
}

}
