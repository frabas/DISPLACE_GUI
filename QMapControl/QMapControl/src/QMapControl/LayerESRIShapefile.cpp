/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2009 - Federico Fuga <fuga@studiofuga.com>
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
* Contact e-mail: fuga@studiofuga.com
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "LayerESRIShapefile.h"

#include "Projection.h"

namespace qmapcontrol {
LayerESRIShapefile::LayerESRIShapefile(const std::string &name, const int &zoom_minimum, const int &zoom_maximum,
                                       QObject *parent)
        : Layer(LayerType::LayerESRIShapefile, name, zoom_minimum, zoom_maximum, parent)
{
}

void
LayerESRIShapefile::addESRIShapefile(const std::shared_ptr<ESRIShapefile> &esri_shapefile, const bool &disable_redraw)
{
    // Check the ESRI Shapefile is valid.
    if (esri_shapefile != nullptr) {
        // Gain a write lock to protect the ESRI Shapefiles.
        QWriteLocker locker(&m_esri_shapefiles_mutex);

        // Find the object in the container.
        const auto itr_find(std::find(m_esri_shapefiles.begin(), m_esri_shapefiles.end(), esri_shapefile));
        if (itr_find != m_esri_shapefiles.end()) {
            // ESRI Shapefile already been added!
        } else {
            // Add the ESRI Shapefile.
            m_esri_shapefiles.push_back(esri_shapefile);

            // Should we redraw?
            if (disable_redraw == false) {
                // Emit to redraw layer.
                emit requestRedraw();
            }

            // ESRI Shapefile can request a redraw, e.g. when its position has been changed.
            // Connect the redraw signal to promulgate up as required.
            QObject::connect(esri_shapefile.get(), &ESRIShapefile::requestRedraw, this, &Layer::requestRedraw);
        }
        }
    }

    void LayerESRIShapefile::removeESRIShapefile(const std::shared_ptr<ESRIShapefile>& esri_shapefile, const bool& disable_redraw)
    {
        // Check the ESRI Shapefile is valid.
        if(esri_shapefile != nullptr)
        {
            // Gain a write lock to protect the ESRI Shapefiles.
            QWriteLocker locker(&m_esri_shapefiles_mutex);

            // Disconnect any signals that were previously connected.
            QObject::disconnect(esri_shapefile.get(), 0, this, 0);

            // Find the object in the container.
            const auto itr_find(std::find(m_esri_shapefiles.begin(), m_esri_shapefiles.end(), esri_shapefile));
            if(itr_find == m_esri_shapefiles.end())
            {
                // ESRI Shapefile does not exist.
            }
            else
            {
                // Remove the object.
                m_esri_shapefiles.erase(itr_find);
            }

            // Should we redraw?
            if(disable_redraw == false)
            {
                // Emit to redraw layer.
                emit requestRedraw();
            }
        }
    }

    void LayerESRIShapefile::clearESRIShapefiles(const bool& disable_redraw)
    {
        // Gain a write lock to protect the ESRI Shapefiles.
        QWriteLocker locker(&m_esri_shapefiles_mutex);

        // Remove all ESRI Shapefiles from the list.
        m_esri_shapefiles.clear();

        // Should we redraw?
        if(disable_redraw == false)
        {
            // Emit to redraw layer.
            emit requestRedraw();
        }
    }

bool LayerESRIShapefile::mousePressEvent(const QMouseEvent *mouse_event, const PointWorldCoord &mouse_point_coord,
                                         const int &controller_zoom) const
{
    // Are mouse events enabled, is the layer visible and is it a mouse press event?
    if (isMouseEventsEnabled() && isVisible(controller_zoom) && mouse_event->type() == QEvent::MouseButtonPress) {
        // Is this a left-click event?
        if (mouse_event->button() == Qt::LeftButton) {
            // Calculate the mouse press world point in pixels.
            const PointWorldPx mouse_point_px(projection::get().toPointWorldPx(mouse_point_coord, controller_zoom));

            // Calculate a rect around the mouse point with a 'fuzzy-factor' around it in pixels.
            const RectWorldPx mouse_rect_px(PointWorldPx(mouse_point_px.x() - mFuzzyFactorPx,
                                                         mouse_point_px.y() - mFuzzyFactorPx),
                                            PointWorldPx(mouse_point_px.x() + mFuzzyFactorPx,
                                                         mouse_point_px.y() + mFuzzyFactorPx));

            // Calculate a rect around the mouse point with a 'fuzzy-factor' around it in coordinates.
            const RectWorldCoord mouse_rect_coord(
                    projection::get().toPointWorldCoord(mouse_rect_px.topLeftPx(), controller_zoom),
                    projection::get().toPointWorldCoord(mouse_rect_px.bottomRightPx(), controller_zoom));

            qreal ff = std::abs(
                    mouse_rect_coord.topLeftCoord().longitude() - mouse_rect_coord.bottomRightCoord().longitude());

            // Create a QGraphicsRectItem to perform touches check, as required.
//                const GeometryPolygon touches_rect_coord({ mouse_rect_coord.topLeftCoord(), mouse_rect_coord.bottomRightCoord() });

            // Check each geometry to see it is contained in our touch area.

            std::vector<OGRFeature *> features;
            m_esri_shapefiles_mutex.lockForRead();
            for (const auto &adapter : m_esri_shapefiles) {
                auto featureInThisAdapter = adapter->findFeatureByRect(mouse_rect_coord);
                std::copy(featureInThisAdapter.begin(), featureInThisAdapter.end(), std::back_inserter(features));
            }
            m_esri_shapefiles_mutex.unlock();

            if (!features.empty()) {
                // here alert of each new feature hit
                emit featuresClicked(features);
                return true;
            }
        }
    }

    return false;
}

    void LayerESRIShapefile::draw(QPainter& painter, const RectWorldPx& backbuffer_rect_px, const int& controller_zoom) const
    {
        // Gain a read lock to protect the ESRI Shapefiles.
        QReadLocker locker(&m_esri_shapefiles_mutex);

        // Check the layer is visible.
        if(isVisible(controller_zoom))
        {
            // Loop through each ESRI Shapefile.
            for(const auto& esri_shapefile : m_esri_shapefiles)
            {
                // Save the current painter's state.
                painter.save();

                // Draw the ESRI Shapefile.
                esri_shapefile->draw(painter, backbuffer_rect_px, controller_zoom);

                // Restore the painter's state.
                painter.restore();
            }
        }
    }

    std::shared_ptr<ESRIShapefile> LayerESRIShapefile::getShapefile(int idx) const
    {
        QReadLocker lock(&m_esri_shapefiles_mutex);
        return m_esri_shapefiles[idx];
    }
}
