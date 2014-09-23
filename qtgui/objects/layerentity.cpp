#include <objects/LayerEntity.h>

#include <objecttreemodel.h>
#include <mapcontrol.h>

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
    return model->createCategoryEntity(0, 0, ObjectTreeModel::Layers);
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
        return model->getMapControl()->numberOfLayers();

    return 0;
}

int LayerEntity::columnCount() const
{
    return 2;
}

QVariant LayerEntity::data(const QModelIndex &index, int role) const
{
    if (mLayerEntityIndex == -1) {
        // boh?
    } else {
        switch (index.column()) {
        case 0:
            if (role != Qt::DisplayRole)
                return QVariant();
            return QString(model->getMapControl()->layerNameAt(index.row()));
        case 1:
            if (role == Qt::CheckStateRole)
                return QVariant(model->getMapControl()->layerAt(index.row())->isVisible() ? Qt::Checked : Qt::Unchecked);
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags LayerEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    if (mLayerEntityIndex != -1 && index.column() == 1) {
        qDebug() << "Index at " << index << " is user checkable";
        return defFlags | Qt::ItemIsUserCheckable;
    }

    return defFlags;
}

bool LayerEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "SetData " << index << value << role;

    if(index.column() == 1 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getMapControl()->layerAt(index.row())->setVisible(false);
        } else {
            model->getMapControl()->layerAt(index.row())->setVisible(true);
        }
        return true;
    }
    return false;
}

}
