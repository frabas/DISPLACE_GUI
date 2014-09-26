#ifndef BENTHOSENTITY_H
#define BENTHOSENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class BenthosEntity : public ObjectTreeEntity
{
    int mId;

public:
    BenthosEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Benthos;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;

};


}

#endif // BENTHOSENTITY_H
