/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
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
        GraphNodeWithCumSweptAreaRole,
        GraphNodeWithCumSubsurfaceSweptAreaRole,
        GraphNodeWithCumCatchesRole,
        GraphNodeWithCumCatchesWithThresholdRole,
        GraphNodeWithCumDiscardsRole,
        GraphNodeWithPopImpact,
        GraphNodeWithPopCumcatches,
        GraphNodeWithBenthosBiomass,
        GraphNodeWithBenthosNumber,
        GraphNodeWithBenthosMeanweight,
        GraphNodeWithTariffs0,
        GraphNodeWithTariffs1,
        GraphNodeWithTariffs2,
        GraphNodeWithSalinity,
        GraphNodeWithSST,
        GraphNodeWithWind,
        GraphNodeWithNitrogen,
        GraphNodeWithPhosphorus,
        GraphNodeWithOxygen,
        GraphNodeWithDissolvedCarbon
    };

    NodeMapObject(MapObjectsController *controller, int indx, Role role, std::shared_ptr<NodeData> node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool showProperties();
    virtual void updateProperties();

    bool selected() const { return mGeometry->selected(); }
    std::shared_ptr<NodeData> node() const { return mNode; }

    Role getRole() const { return mRole; }

protected:
    QString updateStatText(QString prefix);
    QList<int> getInterestingList() const;

private slots:
    void toolButtonClicked();
    void widgetClosed();

private:
    std::shared_ptr<NodeData> mNode;
    Role mRole;
    std::shared_ptr<NodeGraphics> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // NODEMAPOBJECT_H
