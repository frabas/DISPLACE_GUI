#include "metiersentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

using namespace objecttree;

MetiersEntity::MetiersEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model), mId(id)
{
}

QModelIndex MetiersEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Metiers);
}

QModelIndex MetiersEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new MetiersEntity(model, row);
    return model->createEntity(row, column, entity);
}

int MetiersEntity::rowCount() const
{
    if (mId == -1 && model->getModel() != 0)
        return model->getModel()->getMetiersCount();

    return 0;
}

int MetiersEntity::columnCount() const
{
    return 1;
}

QVariant MetiersEntity::data(const QModelIndex &index, int role) const
{
    if (mId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return QString("%1").arg(model->getModel()->getMetiersList()[mId]->getId());
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
