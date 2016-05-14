#ifndef METIERSENTITY_H
#define METIERSENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class MetiersInterest {
public:
    MetiersInterest() {
    }

    bool ticked = false;
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

    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QMenu *contextMenu() const override;

private slots:
    void addMetierActivated();
    void addPopulationActivated();
    void addHarbourActivated();
    void removePopulationActivated();
    void removeHarbourActivated();
    void removeActivated();

private:
    void initMenu();
};

} //namespace

#endif // METIERSENTITY_H
