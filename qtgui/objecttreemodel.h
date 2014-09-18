#ifndef OBJECTTREEMODEL_H
#define OBJECTTREEMODEL_H

#include <QAbstractItemModel>

class ObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    enum Categories {
        Layers = 1, Vessels, Nodes,
        LastCategory
    };

#define CATPOS 28
#define PARCATPOS 24
#define CATMASK 0x0f

public:
    explicit ObjectTreeModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:


protected:
    // helper functions

    bool isRootLevel (const QModelIndex &level) const {
        return !level.isValid();
    }

    bool isCategoryLevel (const QModelIndex &level) const {
        return level.isValid() && catFromId(level.internalId()) != 0;
    }

    bool isObjectLevel (const QModelIndex &level) const {
        return level.isValid() && parCatFromId(level.internalId()) != 0;
    }

    Categories catFromId(quintptr id) const {
        return (Categories)((id >> CATPOS) & CATMASK);
    }
    Categories parCatFromId(quintptr id) const {
        return (Categories)((id >> PARCATPOS) & CATMASK);
    }
    quintptr idWithCat(quintptr id, Categories cat) const {
        return (id & ~(CATMASK << CATPOS)) | (cat << CATPOS);
    }
    quintptr idWithParCat(quintptr id, Categories parcat) const {
        return (id & ~(CATMASK << PARCATPOS)) | (parcat << PARCATPOS);
    }
};

#endif // OBJECTTREEMODEL_H
