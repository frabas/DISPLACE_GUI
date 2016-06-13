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
#ifndef VESSELMAPOBJECT_H
#define VESSELMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/vesseldata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/GeometryPointShapeScaled.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;

class VesselMapObject : public QObject, public MapObject
{
    Q_OBJECT

    class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;
        static QBrush *statHarbour, *statSteaming, *statFishing;

        VesselData *mVessel;
    public:
        VesselGraphics (VesselData *vessel);

        void updated();
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
    };

public:
    VesselMapObject(MapObjectsController *controller, VesselData *vessel);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool showProperties();
    virtual void updateProperties();

    void vesselUpdated();

    static QString vesselStateToString(int state);

private slots:
    void widgetClosed();

private:
    MapObjectsController *mController;

    VesselData *mVessel;
    std::shared_ptr<VesselGraphics> mGeometry;

    NodeDetailsWidget *mWidget;
};

#endif // VESSELMAPOBJECT_H
