#ifndef EDGEMAPOBJECT_H
#define EDGEMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/Geometry.h>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;
class NodeDetailsWidget;

class EdgeGraphics;

class EdgeGraphics : public qmapcontrol::GeometryLineString {
    static QPen mNormalPen, mSelectedPen;
public:
    explicit EdgeGraphics(const std::vector<qmapcontrol::PointWorldCoord>& points);

    virtual void draw(QPainter& painter, const qmapcontrol::RectWorldCoord& backbuffer_rect_coord, const int& controller_zoom);
};

class EdgeMapObject : public QObject, public MapObject
{
    Q_OBJECT

public:
    EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    void onSelectionChanged() override;

    bool selected() const { return mGeometry->selected(); }

    std::shared_ptr<NodeData> node() const { return mNode; }
    std::shared_ptr<NodeData> target() const { return mTarget; }
protected:

private slots:

signals:
    void edgeSelectionHasChanged(EdgeMapObject *object);

private:

    MapObjectsController *mController;
    std::shared_ptr<NodeData> mNode, mTarget;

    std::shared_ptr<EdgeGraphics> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // EDGEMAPOBJECT_H
