#ifndef VESSELENTITY_H
#define VESSELENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class VesselEntity : public ObjectTreeEntity
{
    int mVesselId;

public:
    VesselEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Vessels;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    // Specific access functions
    int getVesselId() const { return mVesselId; }
};

}

#endif // VESSELENTITY_H
