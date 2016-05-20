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
        auto &met = model->getModel()->getMetiersList()[mId];
        if (role == Qt::DisplayRole)
            return met->description();
        if (role == Qt::CheckStateRole)
            return met->ticked ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

Qt::ItemFlags MetiersEntity::flags(Qt::ItemFlags defflags, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return defflags | Qt::ItemIsUserCheckable;
}

bool MetiersEntity::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    if(role == Qt::CheckStateRole) {
        auto &met = model->getModel()->getMetiersList()[mId];
        met->ticked = value.toInt() != 0;
        return true;
    }
    return false;
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
    int mid = QInputDialog::getInt(nullptr, tr("Add Metier"), tr("Metier Id: "), 0, 0, 2147483647, 1, &ok);

    if (ok) {
        model->getModel()->addMetier(mid);
        model->refresh();
    }
}

void MetiersEntity::addPopulationActivated()
{
    bool ok;
    int mid = QInputDialog::getInt(nullptr, tr("Set Population"), tr("Population id: "), 0, 0, 2147483647, 1, &ok);

    if (ok) {
        model->getModel()->getMetiersList()[mId]->populationId = mid;
        model->getModel()->getMetiersList()[mId]->harbourId = -1;
        model->refresh();
    }

}

void MetiersEntity::addHarbourActivated()
{
    bool ok;
    int mid = QInputDialog::getInt(nullptr, tr("Set Harbour"), tr("Harbour id: "), 0, 0, 2147483647, 1, &ok);

    if (ok) {
        model->getModel()->getMetiersList()[mId]->harbourId = mid;
        model->getModel()->getMetiersList()[mId]->populationId = -1;
        model->refresh();
    }
}

void MetiersEntity::removePopulationActivated()
{
    model->getModel()->getMetiersList()[mId]->populationId = -1;
    model->refresh();
}

void MetiersEntity::removeHarbourActivated()
{
    model->getModel()->getMetiersList()[mId]->harbourId = -1;
    model->refresh();
}

void MetiersEntity::removeActivated()
{

}

void MetiersEntity::initMenu()
{
    mContextMenu = new QMenu();

    if (mId == -1) {
        connect (mContextMenu->addAction(QObject::tr("Add Metier...")), SIGNAL(triggered()), this, SLOT(addMetierActivated()));
    } else {
        connect (mContextMenu->addAction(QObject::tr("Add Population...")), SIGNAL(triggered()), this, SLOT(addPopulationActivated()));
        connect (mContextMenu->addAction(QObject::tr("Remove Population")), SIGNAL(triggered()), this, SLOT(removePopulationActivated()));
        connect (mContextMenu->addAction(QObject::tr("Add Harbour...")), SIGNAL(triggered()), this, SLOT(addHarbourActivated()));
        connect (mContextMenu->addAction(QObject::tr("Remove Harbour")), SIGNAL(triggered()), this, SLOT(removeHarbourActivated()));
        mContextMenu->addSeparator();
        connect (mContextMenu->addAction(QObject::tr("Remove")), SIGNAL(triggered()), this, SLOT(removeActivated()));
    }
}
