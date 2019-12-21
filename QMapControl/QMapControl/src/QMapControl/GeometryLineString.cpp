/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "GeometryLineString.h"

// Local includes.
#include "Projection.h"
#include "LayerGeometry.h"

namespace qmapcontrol
{
    GeometryLineString::GeometryLineString(const std::vector<PointWorldCoord>& points, const int& zoom_minimum, const int& zoom_maximum)
        : Geometry(Geometry::GeometryType::GeometryLineString, zoom_minimum, zoom_maximum),
          m_points(points)
    {
    }

    GeometryLineString::GeometryLineString(int zoom_min, int zoom_max)
        : Geometry(Geometry::GeometryType::GeometryLineString, zoom_min, zoom_max),
          m_points()
    {
    }

    const std::vector<PointWorldCoord> &GeometryLineString::points() const
    {
        // Return the points.
        return m_points;
    }

    std::vector<PointWorldCoord> GeometryLineString::quadtree_points() const
    {
        static const int N=11;

        std::vector<PointWorldCoord> coords;
        auto n = m_points.size();

        if (n < 2)
            return m_points;

        for (int i = 1; i < n; ++i) {
            double dx = m_points[i].longitude() - m_points[i-1].longitude();
            double dy = m_points[i].latitude() - m_points[i-1].latitude();

            for (int j = 0; j <=N; ++j) {
                PointWorldCoord pt(m_points[i-1].longitude() + j * dx / N, m_points[i-1].latitude() + j * dy / N);

                coords.push_back(pt);
            }
        }

        return coords;
    }

    void GeometryLineString::addPoint(const PointWorldCoord& point)
    {
        // Add the point.
        m_points.push_back(point);

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }

    void GeometryLineString::setPoints(const std::vector<PointWorldCoord>& points)
    {
        // Set the new points.
        m_points = points;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }

    RectWorldCoord GeometryLineString::boundingBox(const int& /*controller_zoom*/) const
    {
        // Create a polygon of the points.
        QPolygonF polygon_line;

        // Loop through each point to add to the polygon.
        for(const auto& point : m_points)
        {
            // Add the point to be drawn.
            polygon_line.append(point.rawPoint());
        }

        // Return the bounding box.
        return RectWorldCoord::fromQRectF(polygon_line.boundingRect());
    }

    bool GeometryLineString::touches(const Geometry* geometry, const int& controller_zoom) const
    {
        Q_UNUSED(geometry);
        Q_UNUSED(controller_zoom);
        bool return_touches(false);
        return return_touches;
    }

    bool GeometryLineString::hitTestPoint(const PointWorldCoord &point, qreal fuzzyfactor, int controller_zoom) const
    {
        Q_UNUSED(controller_zoom);

        bool return_touches(false);

        // Check the geometry is visible.
        if(isVisible(controller_zoom))
        {
            qreal dx = fuzzyfactor;
            dx = dx*dx;
            qreal d;

            for (size_t i = 1; i < m_points.size(); ++i) {
                d = distToSegmentSquared(point.rawPoint(), m_points[i].rawPoint(), m_points[i-1].rawPoint());

                if (d <= std::abs(fuzzyfactor)) {
                    return_touches = true;
                    break;
                }
            }

            // Did we find at least one geometry touching?
            if(return_touches)
            {
                // Emit that the geometry has been clicked.
                emit geometryClicked(this);
            }
        }

        // Return our success.
        return return_touches;
    }

    /*
    bool GeometryLineString::touches(const Geometry* geometry, const int& controller_zoom) const
    {
        bool return_touches(false);

        // Check the geometry is visible.
        if(isVisible(controller_zoom))
        {
            qreal dx = mLayer->getFuzzyFactorPx();
            dx = dx*dx;
            qreal d;

            for (int i = 1; i < m_points.size() - 1; ++i) {
                d = distToSegmentSquared()

                    return_touches = true;

            }

            // Did we find at least one geometry touching?
            if(return_touches)
            {
                // Emit that the geometry has been clicked.
                emit geometryClicked(this);
            }
        }

        // Return our success.
        return return_touches;
    }*/

    void GeometryLineString::draw(QPainter& painter, const RectWorldCoord& backbuffer_rect_coord, const int& controller_zoom)
    {
        // Check the geometry is visible.
        if(isVisible(controller_zoom))
        {
            // Create a polygon of the points.
            QPolygonF polygon_line;

            // Loop through each point to add to the polygon.
            for(const auto& point : m_points)
            {
                // Add the point to be drawn.
                polygon_line.append(point.rawPoint());
            }

            // Does the polygon intersect with the backbuffer rect?
            if(QPolygonF(backbuffer_rect_coord.rawRect()).intersected(polygon_line).empty() == false)
            {
                // Create a polygon of the points.
                QPolygonF polygon_line_px;

                // Loop through each point to add to the polygon.
                for(const auto& point : m_points)
                {
                    // Add the point to be drawn.
                    polygon_line_px.append(projection::get().toPointWorldPx(point, controller_zoom).rawPoint());
                }

                // Set the pen to use.
                painter.setPen(pen());

                // Draw the polygon line.
                painter.drawPolyline(polygon_line_px);
            }
        }
    }
}
