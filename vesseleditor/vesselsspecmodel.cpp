#include "vesselsspecmodel.h"

#include <vesselsspec.h>

VesselsSpecModel::VesselsSpecModel(QObject *parent)
    : QAbstractItemModel(parent),
      mVesselsSpec(nullptr)
{
}

void VesselsSpecModel::linkVesselsSpec(std::shared_ptr<VesselsSpec> specs)
{
    mVesselsSpec = specs;
//    mVesselsSpec->setOnUpdate(this, [this]() { update(); });
    mVesselsSpec->setOnUpdate(shared_from_this(), std::bind(&VesselsSpecModel::update, this));
}

void VesselsSpecModel::update()
{
    beginResetModel();
    endResetModel();
}


int VesselsSpecModel::rowCount(const QModelIndex &parent) const
{
    return mVesselsSpec->getCount();
}

QVariant VesselsSpecModel::data(const QModelIndex &index, int role) const
{
    auto rec = mVesselsSpec->getRecord(index.row());

    switch (index.column()) {
    case 0:
        return QString::fromStdString(rec.region);
    case 1:
        return QString::fromStdString(rec.harbor);
    case 2:
        return QString::fromStdString(rec.metier);
    }
}

QModelIndex VesselsSpecModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, nullptr);
}

QModelIndex VesselsSpecModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int VesselsSpecModel::columnCount(const QModelIndex &parent) const
{
    return 28;
}
