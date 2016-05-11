#ifndef METIERSENTITY_H
#define METIERSENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class MetiersInterest {
public:
    MetiersInterest() {
    }

    int metierId = -1;
    int populationId = -1;
    int harbourId = -1;

    QString description() const;
    QString description(ObjectTreeModel *model);
};

class MetiersEntity : public QObject, public ObjectTreeEntity
{
    Q_OBJECT

    int mId;
    QMenu *mContextMenu = nullptr;

public:
    MetiersEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::Metiers;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QMenu *contextMenu() const override;

private slots:
    void addMetierActivated();

private:
    void initMenu();
};

} //namespace

#endif // METIERSENTITY_H
