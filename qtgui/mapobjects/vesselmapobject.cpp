#include "vesselmapobject.h"

#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>

#include <QPainter>
#include <QDebug>

VesselMapObject::VesselMapObject(Vessel *vessel)
    : mVessel(vessel)
{
    mGeometry = std::shared_ptr<qmapcontrol::GeometryPointShapeScaled> (new VesselGraphics(mVessel));
}


QBrush *VesselMapObject::VesselGraphics::color = 0;
QBrush *VesselMapObject::VesselGraphics::altColor = 0;

VesselMapObject::VesselGraphics::VesselGraphics(Vessel *vessel)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(vessel->get_x(), vessel->get_y()), QSizeF(20.0, 40.0), 14),
      mVessel(vessel)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);
    if (altColor == 0)
        altColor = new QBrush(Qt::red);
}

void VesselMapObject::VesselGraphics::draw(QPainter &painter, const qmapcontrol::RectWorldCoord &backbuffer_rect_coord, const int &controller_zoom)
{
    // Check the geometry is visible.
    if(isVisible(controller_zoom)) {
        // Check if the bounding boxes intersect.
        const qmapcontrol::RectWorldCoord pixmap_rect_coord(boundingBox(controller_zoom));
        if(backbuffer_rect_coord.rawRect().intersects(pixmap_rect_coord.rawRect()))
        {
            // Calculate the pixmap rect to draw within.
            const qmapcontrol::RectWorldPx pixmap_rect_px(qmapcontrol::projection::get().toPointWorldPx(pixmap_rect_coord.topLeftCoord(), controller_zoom),
                                                          qmapcontrol::projection::get().toPointWorldPx(pixmap_rect_coord.bottomRightCoord(), controller_zoom));

            // Translate to center point with required rotation.
            painter.translate(pixmap_rect_px.centerPx().rawPoint());

            painter.rotate(mVessel->get_course());

            // Draw the shape

            painter.setBrush(*color);
            painter.drawEllipse(-10, -20, 20, 40);

            // Ears
            painter.setBrush(mVessel->get_state() == 3 ? *color : *altColor);
            painter.drawEllipse(-6, -12, 16, 16);

            // Un-translate.
            painter.rotate(-mVessel->get_course());
            painter.translate(-pixmap_rect_px.centerPx().rawPoint());

            // Do we have a meta-data value and should we display it at this zoom?
            if(controller_zoom >= m_metadata_displayed_zoom_minimum && metadata(m_metadata_displayed_key).isNull() == false)
            {
                /// @todo calculate correct alignment for metadata displayed offset.

                // Draw the text next to the point with an offset.
                painter.drawText(pixmap_rect_px.rawRect().topRight() +
                                 qmapcontrol::PointPx(m_metadata_displayed_alignment_offset_px,
                                                      -m_metadata_displayed_alignment_offset_px).rawPoint(),
                                 metadata(m_metadata_displayed_key).toString());
            }
        }
    }

}

