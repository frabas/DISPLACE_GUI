#ifndef NATIONENTITY_H
#define NATIONENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class NationEntity : public ObjectTreeEntity
{
    int mNationIndex;

public:
    NationEntity(ObjectTreeModel *_model, int idx = -1);
    virtual ~NationEntity();

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Nations;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
};

}

#endif // NATIONENTITY_H
