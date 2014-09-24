#include "nodeentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <QMapControl/QMapControl.h>

namespace objecttree {

NodeEntity::NodeEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model),
      mNodeId(id)
{
}

QModelIndex NodeEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Nodes);
}

QModelIndex NodeEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new NodeEntity(model, row);
    return model->createEntity(row, column, entity);
}

int NodeEntity::rowCount() const
{
    if (mNodeId == -1 && model->getModel() != 0)
        return model->getModel()->getNodesCount();

    return 0;
}

int NodeEntity::columnCount() const
{
    return 1;
}

QVariant NodeEntity::data(const QModelIndex &index, int role) const
{
    if (mNodeId != -1 && model->getModel() != 0 && role == Qt::DisplayRole && index.column() == 0) {
        return model->getModel()->getNodeId(mNodeId);
    }

    return QVariant();
}

Qt::ItemFlags NodeEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    return defflags;
}

bool NodeEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;   // item is not editable
}


}
