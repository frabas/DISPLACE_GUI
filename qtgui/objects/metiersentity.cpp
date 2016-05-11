#include "metiersentity.h"

#include <objecttreemodel.h>
#include <displacemodel.h>
#include <mapobjectscontroller.h>

#include <QInputDialog>

using namespace objecttree;


QString MetiersInterest::description() const
{
    QString d = QString(QObject::tr("Met %1")).arg(metierId);

    if (populationId != -1) {
        d.append(QString(QObject::tr(" Pop %1")).arg(populationId));
    }
    if (harbourId != -1) {
        d.append(QString(QObject::tr(" Harb %1")).arg(harbourId));
    }
    return d;
}

QString MetiersInterest::description(ObjectTreeModel *model)
{
    auto m = model->getModel();

    QString d = QString(QObject::tr("Met %1")).arg(metierId);

    if (populationId != -1) {
        d.append(QString(QObject::tr(" Pop %1")).arg(populationId));
    }
    if (harbourId != -1) {
        d.append(QString(QObject::tr(" Harb %1")).arg(m->getHarbourId((harbourId))));
    }
    return d;
}



MetiersEntity::MetiersEntity(ObjectTreeModel *_model, int id)
    : ObjectTreeEntity(_model), mId(id)
{
}

QModelIndex MetiersEntity::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->createCategoryEntityFromChild(ObjectTreeModel::Metiers);
}

QModelIndex MetiersEntity::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    ObjectTreeEntity * entity = new MetiersEntity(model, row);
    return model->createEntity(row, column, entity);
}

int MetiersEntity::rowCount() const
{
    if (mId == -1 && model->getModel() != 0)
        return model->getModel()->getMetiersCount();

    return 0;
}

int MetiersEntity::columnCount() const
{
    return 1;
}

QVariant MetiersEntity::data(const QModelIndex &index, int role) const
{
    if (mId != -1 && model->getModel() != 0 && index.column() == 0) {
        if (role == Qt::DisplayRole)
            return model->getModel()->getMetiersList()[mId]->description();
    }

    return QVariant();
}

QMenu *MetiersEntity::contextMenu() const
{
    if (mContextMenu == nullptr) {
        const_cast<MetiersEntity *>(this)->initMenu();
    }

    return mContextMenu;
}

void MetiersEntity::addMetierActivated()
{
    bool ok;
    int mid = QInputDialog::getInt(nullptr, tr("Add Metier"), tr("Metier to add: "), 0, 0, 2147483647, 1, &ok);

    if (ok) {
        model->getModel()->addMetier(mid);
    }
}

void MetiersEntity::initMenu()
{
    mContextMenu = new QMenu();

    if (mId == -1) {
        connect (mContextMenu->addAction(QObject::tr("Add Metier...")), SIGNAL(triggered()), this, SLOT(addMetierActivated()));
    } else {

    }
}
