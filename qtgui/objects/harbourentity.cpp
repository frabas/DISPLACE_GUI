#include "harbourentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>

namespace objecttree {

HarbourEntity::HarbourEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mHarbourId(id)
{
}

QModelIndex HarbourEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Harbours);
}

QModelIndex HarbourEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new HarbourEntity(model, row);
    return model->createEntity(row, column, entity);
}

int HarbourEntity::rowCount() const
{
    if (mHarbourId == -1 && model->getModel() != 0)
        return model->getModel()->getHarboursCount();

    return 0;
}

int HarbourEntity::columnCount() const
{
    return 1;
}

QVariant HarbourEntity::data(const QModelIndex &index, int role) const
{
    if (mHarbourId != -1 && model->getModel() != 0 && role == Qt::DisplayRole && index.column() == 0) {
        return model->getModel()->getHarbourId(mHarbourId);
    }

    return QVariant();
}

Qt::ItemFlags HarbourEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defflags;
}

bool HarbourEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;   // item is not editable
}

}
