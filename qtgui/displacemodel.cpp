#include "displacemodel.h"

DisplaceModel::DisplaceModel()
{
}

bool DisplaceModel::load(QString path, QString modelname)
{
    mName = modelname;
    mBasePath = path;

    return true;
}
