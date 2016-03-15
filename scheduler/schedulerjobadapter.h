#ifndef SCHEDULERJOBADAPTER_H
#define SCHEDULERJOBADAPTER_H

#include <QObject>
#include <QAbstractItemModel>
#include <schedulerjob.h>

class SchedulerJobAdapter : public QAbstractItemModel
{
    Q_OBJECT

    SchedulerJob &mSj;

    enum Columns { JobName, ModelName, OutputName, NumSteps,
                   ColumnsCount
                 };

public:
    explicit SchedulerJobAdapter(SchedulerJob &sj, QObject *parent = 0);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    QModelIndex parent(const QModelIndex &child) const;
};

#endif // SCHEDULERJOBADAPTER_H
