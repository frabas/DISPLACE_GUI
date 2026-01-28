// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "vesselmapobject.h"

#include <mapobjectscontroller.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/Layer.h>
#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>
#include <QMapControl/ProjectionSphericalMercator.h>// EPSG 3857 projection
#include <QMapControl/ProjectionEquirectangular.h>
#include <QMapControl/LayerGeometry.h>
#include <QObject.h>

#include <calendar.h>


#include <m_constants.h>

#include <QPainter>
#include <QDebug>

class MapObjectsController;
class VesselData;
class NodeDetailsWidget;

VesselMapObject::VesselMapObject(MapObjectsController *controller, VesselData *vessel)
    : mController(controller),
      mVessel(vessel),
      mWidget(0)
{
    mGeometry = std::shared_ptr<VesselGraphics> (new VesselGraphics(mVessel, mController));

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));

    // ----Trajectory line – create with a wide zoom range------------ -
        // The constructor (int zoom_min, int zoom_max) lets us specify the
        // range of zoom levels at which the line is visible.
        // 0 … 22 comfortably covers the whole typical DISPLACE zoom span.
    mTrajectory = std::make_shared<qmapcontrol::GeometryLineString>(0, 22);
    QPen myPen(QColor(0, 120, 255, 180));
    myPen.setWidth(14);
    myPen.setStyle(Qt::SolidLine);   // example: dashed line
    mTrajectory->setPen(myPen);
    mTrajectory->setVisible(true);
    connect(mTrajectory.get(), &qmapcontrol::Geometry::requestRedraw,
        []() { qDebug() << ">>> Geometry requestRedraw received (trajectory)"; });


    // Debug: after creation
    qDebug() << "=== Constructor finished ===";
    dumpDebugInfo(__FUNCTION__);

    // ---- Connect the thread‑safe bridge -------------------------
        // The connection type defaults to Qt::AutoConnection, which becomes
        // Qt::QueuedConnection because the sender (simulation thread) and the
        // receiver (this object) live in different threads.
    connect(this,
        &VesselMapObject::positionReady,
        this,
        &VesselMapObject::onPositionReady,
        Qt::QueuedConnection);   // <-- forces queuing regardless of thread affinity

}

void VesselMapObject::ensureGeometriesAdded()
{
    qDebug() << ">>> ensureGeometriesAdded called";
    dumpDebugInfo(__FUNCTION__); 
    
    int modelIdx = mController->modelIndexForVessel(
        mVessel->mVessel->get_idx());

    if (modelIdx < 0) {
        qWarning() << "ensureGeometriesAdded: could not locate model index for vessel";
        return;
    }

    std::shared_ptr<qmapcontrol::LayerGeometry> entityLayer =
        mController->entityLayer(modelIdx);

    if (!entityLayer) {
        qWarning() << "ensureGeometriesAdded: entity layer is null";
        return;
    }

   
    // **Trajectory layer – separate from the entity layer**
    auto trajLayer = mController->trajectoryLayer(modelIdx);
    if (!trajLayer) {
        qWarning() << "ensureGeometriesAdded: trajectory layer is null";
        return;
    }

    // Add the line to the trajectory layer (drawn first → underneath)
    if (!mTrajectory->layer()) {
        trajLayer->addGeometry(mTrajectory);
        mTrajectory->setVisible(true);   // safety
        // No Z‑value needed – layer order guarantees it is behind the vessel
    }

    // Add the vessel icon to the normal entity layer
    if (!mGeometry->layer())
        entityLayer->addGeometry(mGeometry);
   }


bool VesselMapObject::showProperties()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

QString VesselMapObject::vesselStateToString(int state)
{
    switch (state) {
    case 0:
        return tr("");
    case 1:
        return tr("Fishing");
    case 2:
        return tr("Heading to/Steaming from Harbour");
    default:
        return tr("Unknown?");
    }
}

void VesselMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Coords: </b>%2 %3<br/>")
            .arg(QString::fromStdString(mVessel->mVessel->get_name()))
            .arg(mVessel->mVessel->get_y())
            .arg(mVessel->mVessel->get_x());

    text += "<br/>";

    text += QString("<b>Work hours:</b> %1 - %2<br/>").arg(mVessel->mVessel->getWorkDayStartHour()).arg(mVessel->mVessel->getWorkDayEndHour());
    text += QString("<b>Week End Days:</b> %1 - %2 (%3 - %4)<br/>").arg(Calendar::dayToString(mVessel->mVessel->getWeekEndStartDay())).arg(Calendar::dayToString(mVessel->mVessel->getWeekEndEndDay()))
            .arg(mVessel->mVessel->getWeekEndStartDay()).arg(mVessel->mVessel->getWeekEndEndDay());

    text += "<br/>";
    text += QString("<b>Fuel:</b> %1<br/>").arg(mVessel->mVessel->get_cumfuelcons());
    text += QString("<b>State:</b> %2 (%1)<br/>").arg(mVessel->mVessel->get_state()).arg(vesselStateToString(mVessel->mVessel->get_state()));
    text += QString("<b>Cum Catches:</b> %1<br/>").arg(mVessel->mVessel->get_cumcatches());
    text += QString("<b>Cum Discards:</b> %1<br/>").arg(mVessel->mVessel->get_cumdiscards());
    text += QString("<b>Time at sea:</b> %1<br/>").arg(mVessel->mVessel->get_timeatsea());
    text += QString("<b>Reason To go Back:</b> %1<br/>").arg(mVessel->mVessel->get_reason_to_go_back());

    if (mVessel->mVessel->get_roadmap().size() > 0) {
        text += QString("<b>Final Destination:</b> %1<br/>").arg(mVessel->mVessel->get_roadmap().back().toIndex());
    }

    mWidget->setText(text);
}


void VesselMapObject::VesselGraphics::pushPosition(const QPointF& worldPos)
{
    // Keep the buffer bounded – drop the oldest point when we exceed the limit
    if (mTrajectory.size() >= kMaxPoints)
        mTrajectory.removeFirst();          // FIFO behavior

    mTrajectory.append(worldPos);

    // ---- DEBUG -------------------------------------------------
    qDebug() << "[VesselGraphics] pushPosition – new size =" << mTrajectory.size()
        << "last point =" << mTrajectory.last();

}

void VesselMapObject::recordCurrentPosition()
{
    if (!mGeometry) {
        qWarning() << "[VesselMapObject] No geometry – cannot record position.";
        return;
    }

    // Grab the current geographic coordinates (lon, lat) from the model
    double lon = mVessel->mVessel->get_x();   // longitude in degrees
    double lat = mVessel->mVessel->get_y();   // latitude  in degrees

    // Forward them to the graphics object – this will push them onto the
    // trajectory buffer and emit the redraw signals.
    mGeometry->pushPosition(QPointF(lon, lat));

    // Optional – you can also call the graphics‑object’s updated() so the map
    // refreshes immediately (the same thing the old updated() did).
    mGeometry->updated();
}

// -------------------------------------------------------------------- -
// Slot – runs on the GUI thread, updates the map objects
// ---------------------------------------------------------------------
void VesselMapObject::onPositionReady(const QPointF & lonLat)
{
    // -------------------------------------------------
     // Make sure the geometries belong to a layer (lazy init)
     // -------------------------------------------------
    static bool attached = false;
    if (!attached) {
        ensureGeometriesAdded();
        attached = true;
    }
    // Move the vessel icon
    mGeometry->setCoord(
        qmapcontrol::PointWorldCoord(lonLat.x(), lonLat.y()));

    // The vessel graphics class does not emit requestRedraw() in setCoord(),
    // so we call its helper that does:
    mGeometry->updated();          // emits requestRedraw() → map repaints

    // Append the new point to the trajectory line
    //if (mTrajectory) {
    //    mTrajectory->addPoint(qmapcontrol::PointWorldCoord(lonLat.x(), lonLat.y()));
    //    qDebug() << "Trajectory now has"
    //        << mTrajectory->points().size() << "points";
        // addPoint() automatically emits requestRedraw()
    //}

    // ----- FIFO update -----
    qmapcontrol::PointWorldCoord newPt(lonLat.x(), lonLat.y());
    mTrajectoryBuffer.push_back(newPt);
    if (mTrajectoryBuffer.size() > VesselMapObject::kMaxTrajectoryPoints)
        mTrajectoryBuffer.pop_front();

    // Replace the line's points
    std::vector<qmapcontrol::PointWorldCoord> vec(
        mTrajectoryBuffer.begin(), mTrajectoryBuffer.end());
    
    mTrajectory->setPoints(vec);   // emits requestRedraw()
    if (auto* map = mController->mapWidget())
        map->requestRedraw();      // explicit repaint request

    //qDebug() << "Trajectory now has"
    //    << mTrajectory->points().size() << "points" << " and first lon " << mTrajectory->points().at(0).longitude() << " and lat " << mTrajectory->points().at(0).latitude();

    // Final dump – you should see a non‑null layer, visible = true, correct Z‑value
    dumpDebugInfo(__FUNCTION__);

    // (Optional) keep the old QVector buffer for other uses
    //recordCurrentPosition();
}

// ---------------------------------------------------------------------
// Called from the simulation thread – just forward the data
// ---------------------------------------------------------------------
void VesselMapObject::vesselUpdated()
{
    // Extract the new world coordinates (lon, lat) from the model
    double lon = mVessel->mVessel->get_x();   // degrees
    double lat = mVessel->mVessel->get_y();   // degrees

    qDebug() << ">>> vesselUpdated – emitting positionReady:" << lon << lat;
    
    // Emit the signal – Qt queues the call to onPositionReady() on the GUI thread
    emit positionReady(QPointF(lon, lat));
}



void VesselMapObject::widgetClosed()
{
    mWidget = 0;
}


QBrush *VesselMapObject::VesselGraphics::color = 0;
QBrush *VesselMapObject::VesselGraphics::statFishing= 0;
QBrush *VesselMapObject::VesselGraphics::statHarbour = 0;
QBrush *VesselMapObject::VesselGraphics::statSteaming = 0;


VesselMapObject::VesselGraphics::VesselGraphics(VesselData *vessel, MapObjectsController* controller)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(vessel->mVessel->get_x(), vessel->mVessel->get_y()), QSizeF(20.0, 40.0), 11, 7, 17),
      mVessel(vessel), mController(controller)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);
    if (statHarbour == 0)
        statHarbour = new QBrush(Qt::green);
    if (statFishing == 0)
        statFishing = new QBrush(Qt::red);
    if (statSteaming == 0)
        statSteaming = new QBrush(Qt::black);

    setNonlinearZoomFactor(0.9);
}




void VesselMapObject::VesselGraphics::updated()
{
  
    //setCoord(qmapcontrol::PointWorldCoord(mVessel->mVessel->get_x(),
    //    mVessel->mVessel->get_y()));
    emit positionChanged(this);
    emit requestRedraw();
}


/*
void VesselMapObject::VesselGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.rotate(mVessel->mVessel->get_course());

    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);

    // Ears
    switch (mVessel->mVessel->get_state()) {
    case 1:
        painter.setBrush(*statFishing);
        break;
    case 2:
        painter.setBrush(*statSteaming);
        break;
    default:
    case 3:
        painter.setBrush(*statHarbour);
        break;
    }

    painter.drawEllipse(-6, -12, 12, 24);

    painter.rotate(-mVessel->mVessel->get_course());
}

*/


static const qmapcontrol::Projection* sphericalMercator()
{
    // Construct once, reuse forever – thread‑safe in C++11+
    static const qmapcontrol::ProjectionSphericalMercator proj;
    return &proj;          // EPSG:3857 (Web‑Mercator)
}

static const qmapcontrol::Projection* equirectangular()
{
    // Construct once, reuse forever – thread‑safe in C++11+
    static const qmapcontrol::ProjectionEquirectangular proj;
    return &proj;          
}

static inline qmapcontrol::PointWorldPx lonLatToMercatorMeters(
    const qmapcontrol::PointWorldCoord& lonLat)
{
    // Earth radius used by QMapControl (the same constant is inside the
    // ProjectionSphericalMercator implementation)
    constexpr double R = 6378137.0;   // metres

    double lonRad = lonLat.longitude() * M_PI / 180.0;   // λ in radians
    double latRad = lonLat.latitude() * M_PI / 180.0;   // φ in radians

    double x = R * lonRad;
    double y = R * std::log(std::tan(M_PI / 4.0 + latRad / 2.0));

    return qmapcontrol::PointWorldPx(static_cast<int>(std::round(x)),
        static_cast<int>(std::round(y)));
}


static inline qmapcontrol::PointWorldPx worldToPixelMercator(
    const qmapcontrol::Projection* proj,
    const qmapcontrol::QMapControl* map,
    const qmapcontrol::PointWorldCoord& worldLonLat)   // lon/lat in degrees
{
    // 2️⃣  Current zoom factor (each step doubles the scale)
    double zoom = map->getCurrentZoom();               // e.g. 0, 1, 2, …
    double scale = std::pow(2.0, zoom);            // 2^zoom

    // 1️⃣  Convert lon/lat (degrees) → Mercator “map space” (meters)
    // 2️⃣  Convert lon/lat → *projected* world coordinates
    qmapcontrol::PointWorldPx projPt;
        projPt = lonLatToMercatorMeters(worldLonLat);
    
    // 3️⃣  Centre of the view in the same Mercator space
    qmapcontrol::PointWorldPx centrePx = proj->toPointWorldPx(map->mapFocusPointCoord(), 0);
   
    // 4️⃣  Translate to widget coordinates (origin = top‑left)
    qreal dx = (projPt.x() - centrePx.x()) * scale + map->width() / 2.0;
    qreal dy = (projPt.y() - centrePx.y()) * scale + map->height() / 2.0;

    return qmapcontrol::PointWorldPx(static_cast<int>(dx),
        static_cast<int>(dy));
}




static inline qmapcontrol::PointWorldPx worldToPixelEquirectangular(
    const qmapcontrol::Projection* proj,
    const qmapcontrol::QMapControl* map,
    const qmapcontrol::PointWorldCoord& worldLonLat)   // lon/lat in degrees
{
    // -------------------------------------------------
    // 1 Current zoom factor (each step doubles the scale)
    // -------------------------------------------------
    double zoom = map->getCurrentZoom();               
    //double scale = std::pow(2.0, zoom);         // 2^zoom
    double scale = 1.0;         // avoid double scaling!

    // -------------------------------------------------
    // 2️⃣  Convert lon/lat → equirectangular “world space”
    // -------------------------------------------------
    // The equirectangular projection maps lon/lat (degrees) linearly
    // to X/Y in the projection’s internal coordinate system.
    // QMapControl provides `toPointWorldPx(coord, zoom)` for that.
    
    
    // ---- 2️⃣  Project lon/lat → world coordinates (no zoom) --------------
    qmapcontrol::PointWorldPx projPt;

    // Zoom = 0 → tilesX(0) = 1, tilesY(0) = 1, i.e. the whole world fits in one tile.
     // The projection will therefore return the *raw* world coordinates
     // (lon/lat mapped linearly for Equirectangular, metres for Mercator).
    projPt = proj->toPointWorldPx(worldLonLat, zoom);          // no zoom argument: avoid double scaling!

    // Centre of the view (lon/lat) → same projection space -------
    //qmapcontrol::PointWorldCoord centreGeo = map-> mapFocusPointCoord();   // <-- use the correct method for your version
    // replace by:
    qmapcontrol::RectWorldCoord viewRect = map->getViewportRect();
    double x_center = (viewRect.topRightCoord().longitude() + viewRect.bottomLeftCoord().longitude()) / 2;
    double y_center = (viewRect.topRightCoord().latitude()  + viewRect.bottomLeftCoord().latitude()) /2;
    qmapcontrol::PointWorldCoord centreGeo(x_center, y_center);

    qmapcontrol::PointWorldPx centreProj = proj->toPointWorldPx(centreGeo, zoom);

    // -------------------------------------------------
    // Translate projection coordinates → widget pixels
    // -------------------------------------------------
    // The map widget’s origin (0,0) is the **top‑left** corner.
    // We first compute the offset from the centre, apply the zoom scale,
    // then shift by half the widget size.
    
    qreal dx = (projPt.x() - centreProj.x()) * scale + map->width() / 2.0;
    qreal dy = (projPt.y() - centreProj.y()) *  scale + map->height() / 2.0;
    
    //qDebug() << "   projected world (to widget) =" << (projPt.x() - centreProj.x()) * scale  << (projPt.y() - centreProj.y()) * scale;


    // -------------------------------------------------
    // CALCULATE OFFSET
    // -------------------------------------------------
    
    
    // The map widget provides the current visible rectangle in world coords:
    qmapcontrol::PointWorldCoord topLeft = viewRect.topLeftCoord();

    // Convert both points to *world* pixel coordinates at the current zoom.
    qmapcontrol::PointWorldPx tlPx = proj->toPointWorldPx(topLeft, zoom);

    // The offset is simply the difference between the centre and the top‑left,
    // expressed in screen pixels.
    // The centre of the widget is at (width/2, height/2), so we shift accordingly.
    int offX = static_cast<int>((centreProj.x() - tlPx.x()) - map->width() / 2);
    int offY = static_cast<int>((centreProj.y() - tlPx.y()) - map->height() / 2);

    dx += offX;
    dy += offY;
    

    return qmapcontrol::PointWorldPx(static_cast<int>(dx),
        static_cast<int>(dy));
}



void VesselMapObject::VesselGraphics::drawShape(
    QPainter& painter,
    const qmapcontrol::RectWorldPx& )
{
    
    
    /*
    // -------------------------------------------------
    // 1️⃣  Draw the trajectory (polyline) if we have points
    // -------------------------------------------------
    const auto& pts = trajectory();          // QVector<QPointF> (lon, lat)

    // ---------- DEBUG: how many points do we have? ----------
    qDebug() << "[VesselGraphics] trajectory size =" << pts.size();
    if (!pts.isEmpty()) {
        qDebug() << "   first point (lon,lat) =" << pts.front()
            << "   last point (lon,lat) =" << pts.back();
    }

   

    // ---- Obtain the map widget (needed for zoom/centre) ----
    qmapcontrol::QMapControl* mapCtrl = nullptr;
    if (mController) {
        mapCtrl = qobject_cast<qmapcontrol::QMapControl*>(mController->mapWidget());
    }

    if (!mapCtrl) {
        qWarning() << "[VesselGraphics] Map widget not available – cannot draw trajectory.";
    }



    // ---------- DEBUG: map widget info ----------
    qDebug() << "[VesselGraphics] mapCtrl address =" << static_cast<void*>(mapCtrl);
    qDebug() << "   map width =" << mapCtrl->width()
        << "height =" << mapCtrl->height();

    // Zoom – most QMapControl versions expose `zoom()`. If yours uses
    // `getCurrentZoom()` change the line accordingly.
    double zoom = mapCtrl->getCurrentZoom();               // <-- adjust if needed
    qDebug() << "   current zoom level =" << zoom;

    // Centre of the view (lon/lat in degrees)
    //qmapcontrol::PointWorldCoord centreGeo = mapCtrl->mapFocusPointCoord(); // <-- correct method for most versions
    qmapcontrol::RectWorldCoord viewRect = mapCtrl->getViewportRect();
    double x_center = (viewRect.topRightCoord().longitude() + viewRect.bottomLeftCoord().longitude()) / 2;
    double y_center = (viewRect.topRightCoord().latitude() + viewRect.bottomLeftCoord().latitude()) / 2;
    qmapcontrol::PointWorldCoord centreGeo(x_center, y_center);
    qDebug() << "   map centre (lon,lat) =" << centreGeo.longitude() << centreGeo.latitude();

    // ---- Choose the projection you really want to use ----
    // Uncomment the one you intend to use and comment the other.
    //const qmapcontrol::Projection* proj = sphericalMercator();   // EPSG:3857 (Web‑Mercator)
    const qmapcontrol::Projection* proj = equirectangular(); // EPSG:4326 (Plate‑Carrée)
    //const qmapcontrol::Projection* proj = map->projection();

    // ---------- DEBUG: which projection are we using? ----------
    qDebug() << "[VesselGraphics] using projection type ="
        << (proj == sphericalMercator() ? "SphericalMercator (EPSG:3857)"
            : "Equirectangular (EPSG:4326)");

    // ---- Convert every stored world point (lon/lat) to widget pixels ----
    QPolygonF pixelPoly;
    pixelPoly.reserve(pts.size());

    int pointsInside = 0;   // count how many converted points fall inside the widget

    for (const QPointF& lonLat : pts) {
        // lon/lat in degrees
        qmapcontrol::PointWorldCoord geo(lonLat.x(), lonLat.y());

        // ------- DEBUG: raw geographic coordinate -------
        qDebug() << "[VesselGraphics] processing lon/lat =" << geo.longitude() << geo.latitude();

        // Convert to screen pixel using the helper that matches the projection
        qmapcontrol::PointWorldPx screenPx;
        if (proj == sphericalMercator())
            screenPx = worldToPixelMercator(proj, mapCtrl, geo);
        else
            screenPx = worldToPixelEquirectangular(proj, mapCtrl, geo);

        // ------- DEBUG: projected world coordinate and final pixel -------
        qDebug() << "   projected world (px) =" << screenPx.x() << screenPx.y();

        // Check whether the pixel lies inside the widget rectangle
        bool inside = (screenPx.x() >= 0 && screenPx.x() < mapCtrl->width() &&
            screenPx.y() >= 0 && screenPx.y() < mapCtrl->height());

        if (!inside) {
            qDebug() << "   *** point outside widget bounds ***";
        }
        else {
            ++pointsInside;
        }

        pixelPoly << QPointF(screenPx.x(), screenPx.y());
    }

    // ---------- DEBUG: how many points survived the clipping ----------
    qDebug() << "[VesselGraphics] points inside widget =" << pointsInside
        << "out of" << pts.size();

    if (pixelPoly.isEmpty()) {
        qWarning() << "[VesselGraphics] pixelPoly is empty – nothing will be drawn.";
    }

    // ---- Paint the polyline -------------------------------------------------
    QPen pen(QColor(0, 120, 255, 180));   // semi‑transparent blue
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPolyline(pixelPoly);
    qDebug() << "[VesselGraphics] polyline drawn with" << pixelPoly.size() << "vertices.";

    */
    // -------------------------------------------------
    // 2️⃣  Existing vessel drawing 
    // -------------------------------------------------

    painter.rotate(mVessel->mVessel->get_course());

    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);

    // State‑dependent “ears”
    switch (mVessel->mVessel->get_state())
    {
    case 1: painter.setBrush(*statFishing);   break;
    case 2: painter.setBrush(*statSteaming);  break;
    default:
    case 3: painter.setBrush(*statHarbour);   break;
    }
    painter.drawEllipse(-6, -12, 12, 24);

    painter.rotate(-mVessel->mVessel->get_course());
}



void VesselMapObject::dumpDebugInfo(const char* where) const
{
    qDebug() << "=== DEBUG [" << where << "] ===";

    // 1️⃣  Model index
    int modelIdx = mController->modelIndexForVessel(mVessel->mVessel->get_idx());
    qDebug() << "modelIdx =" << modelIdx;

    // 2️⃣  Entity layer pointer & visibility
    auto entityLayer = mController->entityLayer(modelIdx);
    qDebug() << "entityLayer ptr =" << static_cast<void*>(entityLayer.get())
        << "visible =" << (entityLayer ? entityLayer->isVisible() : false);

    // 3️⃣  Trajectory geometry
    if (mTrajectory) {
        auto* map = mController->mapWidget();
        qDebug() << "Trajectory ptr =" << static_cast<void*>(mTrajectory.get())
            << "layer =" << static_cast<void*>(mTrajectory->layer())
            << "visible =" << mTrajectory->isVisible(map->getCurrentZoom())
            << "points =" << mTrajectory->points().size();
        // Z‑value (requires cast to QGraphicsItem)
        if (QGraphicsItem* gi = dynamic_cast<QGraphicsItem*>(mTrajectory.get())) {
            qDebug() << "Trajectory Z‑value =" << gi->zValue();
        }
    }
    else {
        qDebug() << "Trajectory = nullptr!";
    }

    // 4️⃣  Vessel geometry
    if (mGeometry) {
        auto*  map = mController->mapWidget();
        qDebug() << "Vessel ptr =" << static_cast<void*>(mGeometry.get())
            << "layer =" << static_cast<void*>(mGeometry->layer())
            << "visible =" << mGeometry->isVisible(map->getCurrentZoom())
            << "coord =" << mGeometry->coord().longitude()
            << mGeometry->coord().latitude();
        if (QGraphicsItem* gi = dynamic_cast<QGraphicsItem*>(mGeometry.get())) {
            qDebug() << "Vessel Z‑value =" << gi->zValue();
        }
    }
    else {
        qDebug() << "Vessel = nullptr!";
    }

    // 5️⃣  Map zoom
    if (auto* map = mController->mapWidget()) {
        qDebug() << "Map zoom =" << map->getCurrentZoom()
            << "center =" << map->mapFocusPointCoord().longitude()
            << map->mapFocusPointCoord().latitude();
    }
    else {
        qDebug() << "Map widget = nullptr!";
    }

    qDebug() << "=== END DEBUG ===";
}


