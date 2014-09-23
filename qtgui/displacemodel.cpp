#include "displacemodel.h"

#include <qdebug.h>

DisplaceModel::DisplaceModel()
{
}

bool DisplaceModel::load(QString path, QString modelname)
{
    qDebug() << "Loading model" << modelname << "from folder" << path;

    mName = modelname;
    mBasePath = path;

    return true;
}

int DisplaceModel::getHarboursCount() const
{
    return mHarbours.size();
}

QString DisplaceModel::getHarbourId(int idx) const
{
    return QString(mHarbours.at(idx).get_name().c_str());
}
