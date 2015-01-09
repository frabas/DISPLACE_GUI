#include "benthosentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

namespace objecttree {

BenthosEntity::BenthosEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model), mId(id)
{
}

QModelIndex BenthosEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Benthos);
}

QModelIndex BenthosEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new BenthosEntity(model, row);
    return model->createEntity(row, column, entity);
}

int BenthosEntity::rowCount() const
{
    if (mId == -1 && model->getModel() != 0)
        return model->getModel()->getBenthosCount();

    return 0;
}

int BenthosEntity::columnCount() const
{
    return 1;
}

QVariant BenthosEntity::data(const QModelIndex &index, int role) const
{
    if (mId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return QString("%1").arg(model->getModel()->getBenthosList()[mId]->getId());
        if (role == Qt::TextColorRole)
            return model->getMapControl()->getPalette(model->getModelIdx(), PopulationRole).color(mId);
        /*
        if (role == Qt::ToolTipRole) {
            Vessel *v = model->getModel()->getVesselList()[mVesselId];
            return QString("%1 %2").arg(v->get_y()).arg(v->get_x());
        }*/
    }

    return QVariant();
}

}
