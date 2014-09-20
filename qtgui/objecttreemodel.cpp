#include "objecttreemodel.h"

#include <displacemodel.h>
#include <mapcontrol.h>

ObjectTreeModel::ObjectTreeModel(qmapcontrol::MapControl *map, DisplaceModel *model, QObject *parent) :
    QAbstractItemModel(parent),
    mMapControl(map),
    mModel (model)
{
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (isCategoryLevel(parent)) {
        Categories cat = catFromId(parent.internalId());

        if (cat != Layers && mModel == 0)
            return 0;

        switch (cat) {
        case Layers:
            return 2;
        default:
            return 2;
        }
    }
    // all levels have just one column, for now.
    return 2;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (isRootLevel(parent)) { // Categories level
        return LastCategory-1;  // category starts from 1!
    } else if (isCategoryLevel(parent)) {
        Categories cat = catFromId(parent.internalId());

        if (cat != Layers && mModel == 0)
            return 0;

        switch (cat) {
        case Layers:
            return mMapControl->numberOfLayers();
        case Vessels:
            return mModel->getShipCount();
        default:
            return 4;
        }
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
    if (isCategoryLevel(index)) {
        if (role != Qt::DisplayRole)
            return QVariant();

        quintptr cat = catFromId(index.internalId());

        if (index.column() != 0)
        return QVariant();

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
            switch (index.column()) {
            case 0:
                if (role != Qt::DisplayRole)
                    return QVariant();
                return QString(mMapControl->layerNameAt(index.row()));
            case 1:
                if (role == Qt::CheckStateRole)
                    return QVariant(mMapControl->layerAt(index.row())->isVisible() ? Qt::Checked : Qt::Unchecked);
                return QVariant();
            }
        case Vessels:
            if (index.column() == 0 && role == Qt::DisplayRole)
                return QString(mModel->getShipId(index.row()));
            return QVariant();
        case Nodes:
            if (index.column() != 0 && role == Qt::DisplayRole)
                return QString(tr("Node %1")).arg(index.row());
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags ObjectTreeModel::flags(const QModelIndex &index) const
{
    if(isObjectLevel(index) && index.column() == 1)
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    return QAbstractItemModel::flags(index);
}

bool ObjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(isObjectLevel(index) && index.column() == 1 && role == Qt::CheckStateRole) {
        if (value.toInt() == 0) {
            mMapControl->layerAt(index.row())->setVisible(false);
        } else {
            mMapControl->layerAt(index.row())->setVisible(true);
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void ObjectTreeModel::setCurrentModel(DisplaceModel *model)
{
    beginResetModel();
    mModel = model;
    endResetModel();
}
