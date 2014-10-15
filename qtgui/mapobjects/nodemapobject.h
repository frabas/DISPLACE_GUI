#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>

#include <QObject>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;
class NodeDetailsWidget;

class NodeMapObject : public QObject, public MapObject
{
    Q_OBJECT

    MapObjectsController *mController;
public:
    /* Note to developers: When adding "Roles", add a proper creation case into Constructor.
     * Also check MapObjectsController::createMapObjectsFromModel for proper layer creation.
     * And MapObjectsController should define a proper Output Layer type (OutLayerId)
     *
     * */
    enum Role {
        GraphNodeRole,
        GraphNodeWithPopStatsRole,
        GraphNodeWithBiomass,
        GraphNodeWithCumFTimeRole,
        GraphNodeWithPopImpact,
    };

    NodeMapObject(MapObjectsController *controller, int indx, Role role, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool clicked();
    virtual void update();

private slots:
    void widgetClosed();

private:
    NodeData *mNode;
    Role mRole;
    std::shared_ptr<NodeGraphics> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // NODEMAPOBJECT_H
