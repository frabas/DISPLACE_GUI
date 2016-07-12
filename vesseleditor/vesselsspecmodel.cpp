#include "vesselsspecmodel.h"

#include <vesselsspec.h>

const char *VesselsSpecModel::sHeaders[] = {
    "Region", "Harbour", "Metier", "# Vessels",
    "Crew", "Mean LOA", "Mean GT", "Mean kW",
    "Mean Kg", "Hake Kg", "Sole Kg",
    "Redmullet Kg", "Mantis Kg",
    "Fishing Speed", "Cruise Speed", "Fuel Cons",
    "Avg Storage","Fuel Tank","Trip Duration","Multip Fuel Steaming",
    "multipFuelFishing","multipFuelRet","multipFuelInactive","rangeKm",
    "fuelPrice","weekEndStartDay","weekEndEndDay","workHoursStart","workHoursEnd"
};

VesselsSpecModel::VesselsSpecModel(QObject *parent)
    : QAbstractItemModel(parent),
      mVesselsSpec(nullptr)
{
}

void VesselsSpecModel::linkVesselsSpec(std::shared_ptr<VesselsSpec> specs)
{
    mVesselsSpec = specs;
//    mVesselsSpec->setOnUpdate(this, [this]() { update(); });
    mVesselsSpec->setOnUpdate(shared_from_this(), std::bind(&VesselsSpecModel::update, this));
}

void VesselsSpecModel::update()
{
    beginResetModel();
    endResetModel();
}

int VesselsSpecModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mVesselsSpec->getCount();
}

QVariant VesselsSpecModel::data(const QModelIndex &index, int role) const
{
    auto rec = mVesselsSpec->getRecord(index.row());

    switch (index.column()) {
    case 0:
        return data(rec.region, role);
    case 1:
        return data(rec.harbor, role);
    case 2:
        return data(rec.metier, role);
    case 3:
        return data(rec.nVessels, role);
    case 4:
        return data(rec.crew, role);
    case 5:
        return data(rec.meanLoa, role);
    case 6:
        return data(rec.meanGT, role);
    case 7:
        return data(rec.meanKw, role);
    case 8:
        return data(rec.hakeKg, role);
    case 9:
        return data(rec.soleKg, role);
    case 10:
        return data(rec.redmulletKg, role);
    case 11:
        return data(rec.mantisKg, role);
    case 12:
        return data(rec.fishingSpeed, role);
    case 13:
        return data(rec.cruiseSpeed, role);
    case 14:
        return data(rec.fuelCons, role);
    case 15:
        return data(rec.aveStorage, role);
    case 16:
        return data(rec.fuelTank, role);
    case 17:
        return data(rec.tripDuration, role);
    case 18:
        return data(rec.multipFuelSteaming, role);
    case 19:
        return data(rec.multipFuelFishing, role);
    case 20:
        return data(rec.multipFuelRet, role);
    case 21:
        return data(rec.multipFuelInactive, role);
    case 22:
        return data(rec.rangeKm, role);
    case 23:
        return data(rec.fuelPrice, role);
    case 24:
        return data(rec.weekEndStartDay, role);
    case 25:
        return data(rec.weekEndEndDay, role);
    case 26:
        return data(rec.workHoursStart, role);
    case 27:
        return data(rec.workHoursEnd, role);
    }

    return QVariant();
}

QModelIndex VesselsSpecModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column, nullptr);
}

QModelIndex VesselsSpecModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int VesselsSpecModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 28;
}


Qt::ItemFlags VesselsSpecModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}


bool VesselsSpecModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto rec = mVesselsSpec->getRecord(index.row());

    bool ok;

    switch (index.column()) {
    case 0:
        ok = setData(rec.region, value, role); break;
    case 1:
        ok = setData(rec.harbor, value, role); break;
    case 2:
        ok = setData(rec.metier, value, role); break;
    case 3:
        ok = setData(rec.nVessels, value, role); break;
    case 4:
        ok = setData(rec.crew, value, role); break;
    case 5:
        ok = setData(rec.meanLoa, value, role); break;
    case 6:
        ok = setData(rec.meanGT, value, role); break;
    case 7:
        ok = setData(rec.meanKw, value, role); break;
    case 8:
        ok = setData(rec.hakeKg, value, role); break;
    case 9:
        ok = setData(rec.soleKg, value, role); break;
    case 10:
        ok = setData(rec.redmulletKg, value, role); break;
    case 11:
        ok = setData(rec.mantisKg, value, role); break;
    case 12:
        ok = setData(rec.fishingSpeed, value, role); break;
    case 13:
        ok = setData(rec.cruiseSpeed, value, role); break;
    case 14:
        ok = setData(rec.fuelCons, value, role); break;
    case 15:
        ok = setData(rec.aveStorage, value, role); break;
    case 16:
        ok = setData(rec.fuelTank, value, role); break;
    case 17:
        ok = setData(rec.tripDuration, value, role); break;
    case 18:
        ok = setData(rec.multipFuelSteaming, value, role); break;
    case 19:
        ok = setData(rec.multipFuelFishing, value, role); break;
    case 20:
        ok = setData(rec.multipFuelRet, value, role); break;
    case 21:
        ok = setData(rec.multipFuelInactive, value, role); break;
    case 22:
        ok = setData(rec.rangeKm, value, role); break;
    case 23:
        ok = setData(rec.fuelPrice, value, role); break;
    case 24:
        ok = setData(rec.weekEndStartDay, value, role); break;
    case 25:
        ok = setData(rec.weekEndEndDay, value, role); break;
    case 26:
        ok = setData(rec.workHoursStart, value, role); break;
    case 27:
        ok = setData(rec.workHoursEnd, value, role); break;
    }

    if (ok) {
        mVesselsSpec->setRecord (index.row(), rec);
        emit dataChanged(index, index);
    }

    return ok;
}


QVariant VesselsSpecModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant(sHeaders[section]);
    return QVariant::Invalid;
}

