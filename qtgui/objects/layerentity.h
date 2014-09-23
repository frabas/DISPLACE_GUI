#ifndef LAYERENTITY_H
#define LAYERENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class LayerEntity : public ObjectTreeEntity
{
    int mLayerEntityIndex;

public:
    LayerEntity(ObjectTreeModel *_model, int LayerEntity_idx = -1);
    virtual ~LayerEntity();

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
};

}

#endif // LAYERENTITY_H
