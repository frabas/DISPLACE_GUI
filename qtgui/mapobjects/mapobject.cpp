#include "mapobject.h"

#include <QMapControl/Geometry.h>
#include <QDebug>

MapObject::MapObject()
{
}

MapObject::~MapObject()
{

}

bool MapObject::showProperties()
{
    return false;
}

void MapObject::updateProperties()
{

}

void MapObject::toggleSelection()
{
    setSelection(! getGeometryEntity()->selected());
}

void MapObject::setSelection(bool selection)
{
    getGeometryEntity()->setSelected(selection);
}
