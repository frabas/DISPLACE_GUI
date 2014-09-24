#ifndef NODEENTITY_H
#define NODEENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class NodeEntity : public ObjectTreeEntity
{
    int mNodeId;

public:
    NodeEntity(ObjectTreeModel *_model, int id = -1);

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

};

}

#endif // NODEENTITY_H
