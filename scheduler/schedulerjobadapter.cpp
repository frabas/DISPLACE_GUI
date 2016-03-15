#include "schedulerjobadapter.h"

SchedulerJobAdapter::SchedulerJobAdapter(SchedulerJob &sj, QObject *parent)
    : QAbstractItemModel(parent),
      mSj(sj)
{

}

int SchedulerJobAdapter::rowCount(const QModelIndex &) const
{
    return mSj.jobsCount();
}

int SchedulerJobAdapter::columnCount(const QModelIndex &) const
{
    return ColumnsCount;
}

QVariant SchedulerJobAdapter::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant::Invalid;

    auto &job = mSj.job(index.row());

    switch (index.column()) {
    case JobName:
        return job.getName();
    case ModelName:
        return job.getSimulationName();
    case OutputName:
        return job.getSimulationOutputName();
    case NumSteps:
        return job.getSimulationSteps();
    }

    return QVariant::Invalid;
}

bool SchedulerJobAdapter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return QVariant::Invalid;

    bool r = true;
    auto &job = mSj.job(index.row());

    switch (index.column()) {
    case JobName:
        job.setName(value.toString());
        break;
    case ModelName:
        job.setSimulationName(value.toString());
        break;
    case OutputName:
        job.setSimulationOutputName(value.toString());
        break;
    case NumSteps:
        job.setSimulationSteps(value.toInt(&r));
        break;
    default:
        r = false;
        break;
    }

    if (r)
        emit dataChanged(index, index);
    return r;
}

QVariant SchedulerJobAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant::Invalid;

    switch (section) {
    case JobName:
        return tr("Job Name");
    case ModelName:
        return tr("Model Name");
    case OutputName:
        return tr("Output folder name");
    case NumSteps:
        return tr("Steps");
    }

    return QVariant::Invalid;
}

Qt::ItemFlags SchedulerJobAdapter::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QModelIndex SchedulerJobAdapter::index(int row, int column, const QModelIndex &) const
{
    return QAbstractItemModel::createIndex(row,column,nullptr);
}

bool SchedulerJobAdapter::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count-1);
    while (count > 0) {
        mSj.addAt(row);
        --count;
    }
    endInsertRows();

    return true;
}


QModelIndex SchedulerJobAdapter::parent(const QModelIndex &) const
{
    return QModelIndex();
}
