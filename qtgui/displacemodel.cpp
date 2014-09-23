#include "displacemodel.h"

#include <qdebug.h>

DisplaceModel::DisplaceModel()
{
}

bool DisplaceModel::load(QString path, QString modelname, QString outputname)
{
    qDebug() << "Loading model" << modelname << "from folder" << path;

    mName = modelname;
    mBasePath = path;
    mOutputName = outputname;

    /* Load files ... */

    mScenario = Scenario::readFromFile(mName, mBasePath, mOutputName);

    return true;
}

bool DisplaceModel::save()
{
    if (!mScenario.save(mName, mBasePath, mOutputName))
        return false;

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
Scenario DisplaceModel::scenario() const
{
    return mScenario;
}

void DisplaceModel::setScenario(const Scenario &scenario)
{
    mScenario = scenario;
}

