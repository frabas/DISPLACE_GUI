#include "harbourentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
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
    if (role == Qt::DisplayRole)
        return model->getModel()->getHarbourId(mHarbourId);
    if (role == Qt::CheckStateRole)
        return QVariant(model->getModel()->isInterestingHarb(index.row()) ? Qt::Checked : Qt::Unchecked);
    return QVariant();
}

Qt::ItemFlags HarbourEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defflags | Qt::ItemIsUserCheckable;
}

bool HarbourEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingHarb(index.row());
        } else {
            model->getModel()->setInterestingHarb(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}

}
