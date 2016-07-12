#ifndef VESSELSPECMODEL_H
#define VESSELSPECMODEL_H

#include <QAbstractItemModel>
#include <memory>
#include <QVariant>

class VesselsSpec;

class VesselsSpecModel : public std::enable_shared_from_this<VesselsSpecModel>, public QAbstractItemModel
{
public:
    explicit VesselsSpecModel(QObject *parent = nullptr);

    void linkVesselsSpec(std::shared_ptr<VesselsSpec> specs);

    void update();

private:
    static const char *sHeaders[];

    std::shared_ptr<VesselsSpec> mVesselsSpec;

    QVariant data(const std::string &value, int role, bool ro = false) const {
        if (role == Qt::DisplayRole || (role == Qt::EditRole && !ro))
            return QVariant(QString::fromStdString(value));
        return QVariant::Invalid;
    }

    QVariant data(int value, int role, bool ro = false) const {
        if (role == Qt::DisplayRole || (role == Qt::EditRole && !ro))
            return QVariant(value);
        return QVariant::Invalid;
    }

    QVariant data(float value, int role, bool ro = false) const {
        if (role == Qt::DisplayRole || (role == Qt::EditRole && !ro))
            return QVariant(value);
        return QVariant::Invalid;
    }

    bool setData(std::string &var, const QVariant &value, int role) const {
        if (role == Qt::EditRole) {
            var = value.toString().toStdString();
            return true;
        }
        return false;
    }

    bool setData(int &var, const QVariant &value, int role) const {
        if (role == Qt::EditRole) {
            bool ok;
            var = value.toInt(&ok);
            return ok;
        }
        return false;
    }

    bool setData(float &var, const QVariant &value, int role) const {
        if (role == Qt::EditRole) {
            bool ok;
            var = value.toFloat(&ok);
            return ok;
        }
        return false;
    }

public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int columnCount(const QModelIndex &parent) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // VESSELSPECMODEL_H
