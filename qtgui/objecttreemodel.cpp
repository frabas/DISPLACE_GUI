#include "objecttreemodel.h"

ObjectTreeModel::ObjectTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    // all levels have just one column, for now.
    return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (isRootLevel(parent)) { // Categories level
        return LastCategory;
    } else if (isCategoryLevel(parent)) {
        return parent.row() + 3;
    }

    return 0;
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &child) const
{
    if (isRootLevel(child) || isCategoryLevel(child))
        return QModelIndex();

    return createIndex(0, 0, child.internalId() << 4);
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (isRootLevel(parent)) {
        quint64 id;
        switch (row) {
        case Layers:
            id = LayersId;
            break;
        case Vessels:
            id = VesselsId;
            break;
        case Nodes:
            id = NodesId;
            break;
        }

        return createIndex(row, column, id);
    } else if (isCategoryLevel(parent)) {
        quint64 id = (parent.internalId() >> 4) | row;
        return createIndex (row, column, id);
    }

    return QModelIndex();
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (isCategoryLevel(index)) {
        switch (index.internalId()) {
        case LayersId:
            return QString(tr("Layers"));
        case VesselsId:
            return QString(tr("Vessels"));
        case NodesId:
            return QString(tr("Nodes"));
        }

        return QVariant();
    } else if(isObjectLevel(index)) {
        quint64 type = (index.internalId() << 4) & MaskId;
        switch (type) {
        case LayersId:
            return QString(tr("Layer %1")).arg(index.row());
        case VesselsId:
            return QString(tr("Vessel %1")).arg(index.row());
        case NodesId:
            return QString(tr("Node %1")).arg(index.row());
        }
    }

    return QVariant();
}
