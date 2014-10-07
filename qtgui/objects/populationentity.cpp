#include "populationentity.h"

#include <objecttreemodel.h>
#include <mapobjectscontroller.h>
#include <displacemodel.h>

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

    ObjectTreeEntity * entity = new PopulationEntity(model, row);
    return model->createEntity(row, column, entity);
}

int PopulationEntity::rowCount() const
{
    if (mPopulationIndex == -1 && model->getModel() != 0 && model->getModelIdx() != -1)
        return model->getModel()->getPopulationsCount();

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
            model->getStatsController()->updateStats(model->getModel());
        } else {
            model->getModel()->setInterestingPop(index.row());
            model->getStatsController()->updateStats(model->getModel());
        }
        return true;
    }
    return false;
}

}
