#include "csvtablemodel.h"

#include <QDebug>

CsvTableModel::CsvTableModel(std::shared_ptr<QList<QStringList> > data, QObject *parent) :
    QAbstractTableModel(parent),
    mHeaders(false),
    mData(data),
    mRowCount(0),
    mColCount(0)
{
    scan();
}

QVariant CsvTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    int idx = mHeaders ? index.row() +1 : index.row();

    if (mData->at(idx).size() <= index.column())
        return QVariant();

    return mData->at(idx).at(index.column());
}

bool CsvTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    int row = mHeaders ? index.row() + 1 : index.row();
    while (mData->at(row).size() <= index.column())
        (*mData)[row].insert(mData->at(row).size(), QString());

    (*mData)[row][index.column()] = value.toString();
    emit dataChanged(index, index);
    return true;
}

QVariant CsvTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical) {
        return section;
    } else {
        if (mHeaders) {
            if (mData->size() < 1 || mData->at(0).size() <= section)
                return section;
            return mData->at(0).at(section);
        }
    }

    return QVariant();
}

Qt::ItemFlags CsvTableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool CsvTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    int hdr = mHeaders ? 1 : 0;
    beginInsertRows(parent, row+hdr, row+count+hdr-1);
    for (int i = 0; i < count; ++i)
        mData->insert(row + hdr + i, QStringList());
    endInsertRows();

    return true;
}

bool CsvTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    int hdr = mHeaders ? 1 : 0;
    beginRemoveRows(parent, row+hdr, row+count+hdr-1);
    for (int i = 0; i < count; ++i)
        mData->removeAt(row+hdr);
    endRemoveRows();

    return true;
}

void CsvTableModel::setSource(std::shared_ptr<QList<QStringList> > data)
{
    mData = data;
    scan();
}

void CsvTableModel::setFirstLineHeaders(bool x)
{
    beginResetModel();
    mHeaders = x;
    endResetModel();
}

void CsvTableModel::scan()
{
    beginResetModel();

    if (!mData) {
        mColCount = 0;
        mRowCount = 0;
        endResetModel();
        return;
    }

    mRowCount = mData->size();
    mColCount = 0;
    foreach (QStringList x, *mData) {
        if(mColCount < x.size())
            mColCount = x.size();
    }

    endResetModel();
}

