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

    void addSimulationRun(const SimulationRun &data);
    void refresh();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(std::vector<int> rows, const QModelIndex &parent);
    QModelIndex parent(const QModelIndex &child) const override;
};

#endif // SCHEDULERJOBADAPTER_H
