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
//    connect (edge, SIGNAL(selected(EdgeMapObject*,bool)), this, SLOT(edgeSelected(EdgeMapObject*,bool)));

    connect(edge, SIGNAL(selected(bool)), this, SLOT(edgeSelected(bool)));
}

void EdgeLayer::edgeSelected(/*EdgeMapObject *obj, */ bool stat)
{
    qDebug() << "edge selected " << stat << mSelection.size();
    /*
    if (stat)
        mSelection.append(obj);
    else
        mSelection.removeAll(obj);
*/

    emit edgeSelectionChanged(mSelection.size());
}
