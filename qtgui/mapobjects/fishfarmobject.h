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
#ifndef FISHFARMMAPOBJECT_H
#define FISHFARMMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/fishfarmdata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/GeometryPointShapeScaled.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;

class FishfarmMapObject : public QObject, public MapObject
{
    Q_OBJECT

    class FishfarmGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;

        FishfarmData *mFishfarm;
    public:
        FishfarmGraphics (FishfarmData *fishfarm);

        void updated();
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
    };

public:
    FishfarmMapObject(MapObjectsController *controller, FishfarmData *fishfarm);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool showProperties();
    virtual void updateProperties();

    void fishfarmUpdated();

private slots:
    void widgetClosed();

private:
    MapObjectsController *mController;

    FishfarmData *mFishfarm;
    std::shared_ptr<FishfarmGraphics> mGeometry;

    NodeDetailsWidget *mWidget;
};

#endif // FISHFARMMAPOBJECT_H
