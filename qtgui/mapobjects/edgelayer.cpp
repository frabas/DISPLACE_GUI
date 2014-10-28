#include "edgelayer.h"

#include <mapobjects/edgemapobject.h>
#include <mapobjectscontroller.h>

EdgeLayer::EdgeLayer(MapObjectsController *controller, const QString &name, QObject *parent) :
    QObject(parent),
    mController(controller),
    mLayer(new qmapcontrol::LayerGeometry(name.toStdString()))
{
}

void EdgeLayer::addEdge(EdgeMapObject *edge )
{
    mLayer->addGeometry(edge->getGeometryEntity());
}

void EdgeLayer::removeEdge(EdgeMapObject *edge)
{
    mLayer->removeGeometry(edge->getGeometryEntity());
}
