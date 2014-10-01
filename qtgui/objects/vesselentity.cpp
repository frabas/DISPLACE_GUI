#include "vesselentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>

namespace objecttree {

VesselEntity::VesselEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mVesselId(id)
{
}

QModelIndex VesselEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Vessels);
}

QModelIndex VesselEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new VesselEntity(model, row);
    return model->createEntity(row, column, entity);
}

int VesselEntity::rowCount() const
{
    if (mVesselId == -1 && model->getModel() != 0)
        return model->getModel()->getVesselCount();

    return 0;
}

int VesselEntity::columnCount() const
{
    return 1;
}

QVariant VesselEntity::data(const QModelIndex &index, int role) const
{
    if (mVesselId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return model->getModel()->getVesselId(mVesselId);
        if (role == Qt::ToolTipRole) {
            VesselData *v = model->getModel()->getVesselList()[mVesselId];
            return QString("%1 %2").arg(v->mVessel->get_y()).arg(v->mVessel->get_x());
        }
    }

    return QVariant();
}

}
