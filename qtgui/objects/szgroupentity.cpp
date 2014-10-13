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

    ObjectTreeEntity * entity;
    if (mGroupIndex == -1) {
        entity = new SzGroupEntity(model, row);
    }

    return model->createEntity(row, column, entity);
}

int SzGroupEntity::rowCount() const
{
    if (model->getModel() != 0 && model->getModelIdx() != -1) {
        if (mGroupIndex == -1)
            return model->getModel()->getSzGrupsCount() + 1;
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
        if (index.row() == 0) {
            return QString(QObject::tr("Total"));
        }
        return QString(QObject::tr("Size Group #%1")).arg(index.row() - 1);
    }

    if (role == Qt::CheckStateRole) {
        // Note: index.row() == 0 means ALL
        return Qt::Unchecked;
        //return QVariant(model->getModel()->isInterestingPop(index.row()) ? Qt::Checked : Qt::Unchecked);
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
    /*
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            model->getModel()->remInterestingPop(index.row());
        } else {
            model->getModel()->setInterestingPop(index.row());
        }
        model->getStatsController()->updateStats(model->getModel());
        model->getMapControl()->updateNodes(model->getModelIdx());
        return true;
    }*/
    return false;
}

}
