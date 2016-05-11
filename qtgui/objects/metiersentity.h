#ifndef METIERSENTITY_H
#define METIERSENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class MetiersEntity : public ObjectTreeEntity
{
    int mId;

public:
    MetiersEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Metiers;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;

};

} //namespace

#endif // METIERSENTITY_H
