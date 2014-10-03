#ifndef HARBOURENTITY_H
#define HARBOURENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class HarbourEntity : public ObjectTreeEntity
{
    int mHarbourId;
public:
    HarbourEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Harbours;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    // Specific access functions
    int getHarbourId() const { return mHarbourId; }
};

}

#endif // HARBOURENTITY_H
