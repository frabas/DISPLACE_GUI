#ifndef POPULATIONENTITY_H
#define POPULATIONENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class PopulationEntity : public ObjectTreeEntity
{
    int mPopulationIndex;

public:
    PopulationEntity(ObjectTreeModel *_model, int idx = -1);
    virtual ~PopulationEntity();

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Layers;
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

#endif // POPULATIONENTITY_H
