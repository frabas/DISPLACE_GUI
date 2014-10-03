#include "objecttreeentity.h"

namespace objecttree {

ObjectTreeEntity::ObjectTreeEntity(ObjectTreeModel *_model)
    : model(_model)
{
}

ObjectTreeEntity::~ObjectTreeEntity()
{

}

Qt::ItemFlags ObjectTreeEntity::flags(Qt::ItemFlags defFlags, const QModelIndex &index) const
{
    Q_UNUSED(index);

    return defFlags;
}

bool ObjectTreeEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}


}
