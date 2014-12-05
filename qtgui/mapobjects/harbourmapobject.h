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
#ifndef HARBOURMAPOBJECT_H
#define HARBOURMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointImageScaled.h>
#include <modelobjects/harbourdata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QPixmap>

class MapObjectsController;
class DisplaceModel;

class HarbourMapObject : public QObject, public MapObject
{
    Q_OBJECT
public:
    HarbourMapObject(MapObjectsController *controller, DisplaceModel *model, HarbourData *harbour);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool showProperties();
    virtual void updateProperties();

private slots:
    void widgetClosed();

private:
    MapObjectsController *mController;
    DisplaceModel *mModel;

    HarbourData *mHarbour;
    std::shared_ptr<qmapcontrol::GeometryPointImageScaled> mGeometry;

    static QPixmap *symbol;
    NodeDetailsWidget *mWidget;

};

#endif // HARBOURMAPOBJECT_H
