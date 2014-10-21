#ifndef EDGEMAPOBJECT_H
#define EDGEMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>
#include <QMapControl/GeometryLineString.h>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;
class NodeDetailsWidget;

class EdgeMapObject : public QObject, public MapObject
{
    Q_OBJECT

public:
    EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

//    virtual bool clicked();
//    virtual void update();

protected:
//    QString updateStatText(QString prefix);

private slots:
//    void widgetClosed();

private:
    MapObjectsController *mController;
    NodeData *mNode;
    int mEdgeIndex;

    std::shared_ptr<qmapcontrol::GeometryLineString> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // EDGEMAPOBJECT_H
