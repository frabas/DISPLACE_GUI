#ifndef OBJECTTREEMODEL_H
#define OBJECTTREEMODEL_H

#include <QAbstractItemModel>

class ObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    enum Categories {
        Layers = 0, Vessels, Nodes,
        LastCategory
    };

    enum CategoriesId {
        LayersId  = 0x1000000000000000,
        VesselsId = 0x2000000000000000,
        NodesId   = 0x4000000000000000,

        ObjTypeId = 0x0f00000000000000,
        MaskId    = 0xf000000000000000
    };

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
        return level.isValid() && (level.internalId() & MaskId) != 0;
    }

    bool isObjectLevel (const QModelIndex &level) const {
        return level.isValid() && (level.internalId() & MaskId) == 0;
    }
};

#endif // OBJECTTREEMODEL_H
