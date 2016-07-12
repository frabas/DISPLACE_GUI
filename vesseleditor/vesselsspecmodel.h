#ifndef VESSELSPECMODEL_H
#define VESSELSPECMODEL_H

#include <QAbstractItemModel>
#include <memory>

class VesselsSpec;

class VesselsSpecModel : public std::enable_shared_from_this<VesselsSpecModel>, public QAbstractItemModel
{
public:
    explicit VesselsSpecModel(QObject *parent = nullptr);

    void linkVesselsSpec(std::shared_ptr<VesselsSpec> specs);

    void update();

private:
    std::shared_ptr<VesselsSpec> mVesselsSpec;

public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int columnCount(const QModelIndex &parent) const override;
};

#endif // VESSELSPECMODEL_H
