#ifndef SZGROUPENTITY_H
#define SZGROUPENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class SzGroupEntity : public ObjectTreeEntity
{
    int mGroupIndex;

public:
    enum SpecialGroup { Total, Average, Min, Max,
                        LastSpecialGroup };

    SzGroupEntity(ObjectTreeModel *_model, int idx = -1);
    virtual ~SzGroupEntity();

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Populations;
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

#endif // SZGROUPENTITY_H
