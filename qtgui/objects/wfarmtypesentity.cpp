#include "wfarmtypesentity.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>

using namespace objecttree;

WFarmTypesEntity::WFarmTypesEntity(ObjectTreeModel *_model, std::shared_ptr<InterestingListWithSpecialValues<int> > list, int idx)
    : ObjectTreeEntity(_model),
      mGroupIndex(idx),
      mList(list)
{
}

void WFarmTypesEntity::setInterestingList(std::shared_ptr<InterestingListWithSpecialValues<int> > list)
{
    mList = list;
}

QModelIndex WFarmTypesEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::WindfarmsTypes);
}

QModelIndex WFarmTypesEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = 0;
    if (mGroupIndex == -1) {
        entity = new WFarmTypesEntity(model, mList, row);
    }

    return model->createEntity(row, column, entity);
}

int WFarmTypesEntity::rowCount() const
{
    if (model->getModel() != 0 && model->getModelIdx() != -1) {
        if (mGroupIndex == -1)
            return mList->getSpecialValuesCount() + model->getModel()->getNumWindfarmsTypes();    // Total, Average
    }

    return 0;
}

int WFarmTypesEntity::columnCount() const
{
    return 1;
}

QVariant WFarmTypesEntity::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if ((size_t)index.row() < mList->getSpecialValuesCount())
            return mList->getSpecialName(index.row());
        else
            return mList->getFormatString().arg(index.row() - mList->getSpecialValuesCount());
    }

    if (role == Qt::CheckStateRole) {
        if ((size_t)index.row() < mList->getSpecialValuesCount())
            return mList->isSpecialValueSelected(index.row()) ? Qt::Checked : Qt::Unchecked;
        else
            return mList->has(index.row() - mList->getSpecialValuesCount()) ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

Qt::ItemFlags WFarmTypesEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defFlags | Qt::ItemIsUserCheckable;
}

bool WFarmTypesEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        bool sel = value.toInt() != 0;
        if ((size_t)index.row() < mList->getSpecialValuesCount())
            mList->setSpecialValueSelected(index.row(), sel);
        else {
            if (sel)
                mList->set(index.row() - mList->getSpecialValuesCount());
            else
                mList->rem(index.row() - mList->getSpecialValuesCount());
        }
        model->getStatsController()->updateStats(model->getModel());
        //model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }
    return false;
}



void objecttree::WFarmTypesEntity::modelChanged(int idx)
{
    Q_UNUSED(idx);
    if (model && model->getModel())
        mList = model->getModel()->getWindfarmsTypesList();
}
