#ifndef OBJECTTREEENTITY_H
#define OBJECTTREEENTITY_H

#include <QModelIndex>
#include <objecttreemodel.h>

class ObjectTreeModel;

namespace objecttree {

class ObjectTreeEntity
{
protected:
    ObjectTreeModel *model;

public:
    ObjectTreeEntity(ObjectTreeModel *_model);
    virtual ~ObjectTreeEntity();

    virtual ObjectTreeModel::Category getCategory() const = 0;

    virtual QModelIndex parent (const QModelIndex &parent) const = 0;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const = 0;
    virtual int rowCount() const = 0;
    virtual int columnCount() const = 0;
    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    virtual Qt::ItemFlags flags(Qt::ItemFlags defFlags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
};

}

#endif // OBJECTTREEENTITY_H
