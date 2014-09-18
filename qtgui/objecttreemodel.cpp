#include "objecttreemodel.h"

ObjectTreeModel::ObjectTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    // all levels have just one column, for now.
    return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (isRootLevel(parent)) { // Categories level
        return LastCategory-1;  // category starts from 1!
    } else if (isCategoryLevel(parent)) {
        return parent.row() + 3;
    }

    return 0;
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &child) const
{
    if (isRootLevel(child) || isCategoryLevel(child))
        return QModelIndex();

    return createIndex(0, 0, idWithCat(0, parCatFromId(child.internalId())));
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (isRootLevel(parent)) {
        quint64 id = idWithCat(0, (Categories)(row+1));
        return createIndex(row, column, id);
    } else if (isCategoryLevel(parent)) {
        quint64 id = idWithParCat(row, catFromId(parent.internalId()));
        return createIndex (row, column, id);
    }

    return QModelIndex();
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (isCategoryLevel(index)) {
        quintptr cat = catFromId(index.internalId());

        switch (cat) {
        case Layers:
            return QString(tr("Layers"));
        case Vessels:
            return QString(tr("Vessels"));
        case Nodes:
            return QString(tr("Nodes"));
        }

        return QVariant();
    } else if(isObjectLevel(index)) {
        quint64 type = parCatFromId(index.internalId());
        switch (type) {
        case Layers:
            return QString(tr("Layer %1")).arg(index.row());
        case Vessels:
            return QString(tr("Vessel %1")).arg(index.row());
        case Nodes:
            return QString(tr("Node %1")).arg(index.row());
        }
    }

    return QVariant();
}
