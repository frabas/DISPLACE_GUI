#include "displacemodel.h"
#include <objects/Ships.h>

#include <qdebug.h>

DisplaceModel::DisplaceModel()
{
}

bool DisplaceModel::load(QString path, QString modelname)
{
    qDebug() << "Loading model" << modelname << "from folder" << path;

    mName = modelname;
    mBasePath = path;

    mShips = Ship::readFromFile(mName, mBasePath);

    return true;
}

int DisplaceModel::getShipCount() const
{
    return mShips.size();
}

QString DisplaceModel::getShipId(int idx) const
{
    return mShips.at(idx).name();
}
