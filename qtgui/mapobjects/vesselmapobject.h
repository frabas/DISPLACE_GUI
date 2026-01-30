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
#include <deque>

#include <mapobjects/mapobject.h>
#include <modelobjects/vesseldata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/GeometryPointShapeScaled.h>
#include <QMapControl/GeometryLineString.h>   // or GeometryPolyLine.h

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;

class VesselMapObject : public QObject, public MapObject
{
    Q_OBJECT

 /*   class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;
        static QBrush *statHarbour, *statSteaming, *statFishing;

        VesselData *mVessel;
    public:
        VesselGraphics (VesselData *vessel);

        void updated();
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
  



   };
   */

   // Called by MapObjectsController when the vessel moves
        void recordCurrentPosition();


        class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled
    {
    public:
        explicit VesselGraphics(VesselData* vessel, MapObjectsController* controller);
        
       
        void updated();
        virtual void drawShape(QPainter& painter,
            const qmapcontrol::RectWorldPx& rect);

        // ---------- NEW ----------
        /** Append the current position to the trajectory buffer. */
        void pushPosition(const QPointF& worldPos);

        /** Return a const reference for drawing. */
        const QVector<QPointF>& trajectory() const { return mTrajectory; }

    private:
        VesselData* mVessel;
        MapObjectsController* mController;
        static QBrush* color;
        static QBrush* statFishing;
        static QBrush* statHarbour;
        static QBrush* statSteaming;

        /** Buffer that holds the last N world‑coordinates. */
        QVector<QPointF> mTrajectory;
        /** Maximum number of points kept – adjust to UI needs. */
        static constexpr size_t kMaxPoints = 12;
    };


public:
    VesselMapObject(MapObjectsController *controller, VesselData *vessel);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }
    std::shared_ptr<qmapcontrol::GeometryLineString> trajectoryGeometry() const
    {
        return mTrajectory;
    }

    virtual bool showProperties();
    virtual void updateProperties();

    void vesselUpdated();

    static QString vesselStateToString(int state);

signals:
    // Emitted from the simulation thread, delivered to the GUI thread
    void positionReady(const QPointF& lonLat);

private slots:
    void widgetClosed();
    // Runs in the GUI thread – updates the map geometry safely
    void onPositionReady(const QPointF& lonLat);

private:
    MapObjectsController *mController = nullptr;

    // Helper that makes sure the geometries are attached to a map layer
    void ensureGeometriesAdded();

    VesselData *mVessel = nullptr;
    std::shared_ptr<VesselGraphics> mGeometry;
    std::shared_ptr<qmapcontrol::GeometryLineString> mTrajectory;
    NodeDetailsWidget *mWidget = nullptr;
    // -------------------------------------------------
    // FIFO buffer for the trajectory (option B)
    // -------------------------------------------------
    static constexpr std::size_t kMaxTrajectoryPoints = 5;   // choose any limit
    std::deque<qmapcontrol::PointWorldCoord> mTrajectoryBuffer;

    void dumpDebugInfo(const char* where) const;
};

#endif // VESSELMAPOBJECT_H
