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

#include "ESRIShapefile.h"
#include "Projection.h"

#include <QDebug>
#include <QPainterPath>

#include <cmath>
#include <system_error>

namespace qmapcontrol
{
ESRIShapefile::ESRIShapefile(const std::string &file_path, const std::string &layer_name, const int &zoom_minimum,
                             const int &zoom_maximum)
        : m_layer_name(layer_name), m_zoom_minimum(zoom_minimum), m_zoom_maximum(zoom_maximum)
{
    // Register OGR drivers.
    OGRRegisterAll();

    // Open the file.
    m_ogr_data_set = reinterpret_cast<GDALDataset *>(OGROpen(file_path.c_str(), 0, nullptr));
}

ESRIShapefile::ESRIShapefile(GDALDataset *datasource, const std::string &layer_name, const int &zoom_minimum,
                             const int &zoom_maximum)
        : m_ogr_data_set(datasource), m_layer_name(layer_name), m_zoom_minimum(zoom_minimum),
          m_zoom_maximum(zoom_maximum)
{
}

void ESRIShapefile::createProjections(OGRSpatialReference *spatialReference) const
{
    OGRSpatialReference destinationWCS;
    // TODO check the correct destination WCS.
    if (destinationWCS.importFromEPSG(4326) != OGRERR_NONE) {
        //throw std::runtime_error("Can't import EPSG");
    }
#if GDAL_VERSION_MAJOR >= 3
    destinationWCS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
#endif

    if (spatialReference == nullptr) {
        spatialReference = &destinationWCS;
    }

    mTransformation = OGRCreateCoordinateTransformation(spatialReference, &destinationWCS);
    mInvTransformation = OGRCreateCoordinateTransformation(&destinationWCS, spatialReference);
}

ESRIShapefile::~ESRIShapefile()
{
    if (m_ogr_data_set != nullptr && hasDatasetOwnership) {
        GDALClose(m_ogr_data_set);
    }
}

const QPen &ESRIShapefile::getPen() const
{
    return mPen;
}

void ESRIShapefile::setPen(QPen pen)
{
    // Set the pen to draw with.
    mPen = std::move(pen);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

const QBrush &ESRIShapefile::getBrush() const
{
    return mBrush;
}

void ESRIShapefile::setBrush(QBrush brush)
{
    // Set the brush to draw with.
    mBrush = std::move(brush);

    // Emit that we need to redraw to display this change.
    emit requestRedraw();
}

void ESRIShapefile::draw(QPainter &painter, const RectWorldPx &backbuffer_rect_px, const int &controller_zoom) const
{
    // Check whether the controller zoom is within range?
    if (m_zoom_minimum > controller_zoom || m_zoom_maximum < controller_zoom) {
        // Outside of supported zoom levels.
    } else {
        // Calculate the world coordinates.
        const RectWorldCoord backbuffer_rect_coord(
                projection::get().toPointWorldCoord(backbuffer_rect_px.topLeftPx(), controller_zoom),
                projection::get().toPointWorldCoord(backbuffer_rect_px.bottomRightPx(), controller_zoom));

        // Do we have a data set open?
        if (m_ogr_data_set != nullptr) {
            // Do we have a layer name set?
            if (m_layer_name.empty() == false) {
                // Get layer.
                const auto ogr_layer(m_ogr_data_set->GetLayerByName(m_layer_name.c_str()));
                if (ogr_layer == nullptr) {
                    // Invalid layer name!
                } else {
                    if (mTransformation == nullptr) {
                        createProjections(ogr_layer->GetSpatialRef());
                    }

                    auto points = backbuffer_rect_coord.toStdVector();
                    std::vector<double> xs, ys;
                    std::transform(points.begin(), points.end(), std::back_inserter(xs),
                                   [](PointWorldCoord const &pt) {
                                       return pt.rawPoint().x();
                                   });
                    std::transform(points.begin(), points.end(), std::back_inserter(ys),
                                   [](PointWorldCoord const &pt) {
                                       return pt.rawPoint().y();
                                   });

                    mInvTransformation->Transform(points.size(), xs.data(), ys.data());

                        // Note: sequence is: topleft, topright, botRIGHT, botleft
//                        qDebug() << "Window " << backbuffer_rect_coord.rawRect() << " => "
//                                 << xs[0] << ys[0] << xs[2] << ys[2];

                    // Reset reading.
                    ogr_layer->ResetReading();

                    // Set the Spatial Filter.
                    ogr_layer->SetSpatialFilterRect(xs[0],
                                                    ys[0],
                                                    xs[2],
                                                    ys[2]);

                    if (!attributeFilter.empty()) {
                        ogr_layer->SetAttributeFilter(attributeFilter.c_str());
                    }

                    // Loop through features.
                    OGRFeature *ogr_feature;
                    while ((ogr_feature = ogr_layer->GetNextFeature()) != nullptr) {
                        // Draw the feature.
                        drawFeature(ogr_feature, painter, controller_zoom);

                        // Destroy the feature.
                        OGRFeature::DestroyFeature(ogr_feature);
                    }
                }
                }
                else
                {
                    // Loop through and draw each layer.
                    for (int i = 0; i < m_ogr_data_set->GetLayerCount(); ++i) {
                        // Get layer.
                        const auto ogr_layer(m_ogr_data_set->GetLayer(i));
                        if (ogr_layer != nullptr) {
                            if (mTransformation) {
                                delete mTransformation;
                            }
                            createProjections(ogr_layer->GetSpatialRef());

                            auto points = backbuffer_rect_coord.toStdVector();
                            std::vector<double> xs, ys;
                            std::transform(points.begin(), points.end(), std::back_inserter(xs),
                                           [](PointWorldCoord const &pt) {
                                               return pt.rawPoint().x();
                                           });
                            std::transform(points.begin(), points.end(), std::back_inserter(ys),
                                           [](PointWorldCoord const &pt) {
                                               return pt.rawPoint().y();
                                           });

                            // Note: sequence is: topleft, topright, botRIGHT, botleft
                            mInvTransformation->Transform(points.size(), xs.data(), ys.data());

//                            qDebug() << "Window " << backbuffer_rect_coord.rawRect() << " => "
//                                     << xs[0] << ys[0] << xs[2] << ys[2];
//

                            // Reset reading.
                            ogr_layer->ResetReading();

                            // Set the Spatial Filter.
                            ogr_layer->SetSpatialFilterRect(xs[0],
                                                            ys[0],
                                                            xs[2],
                                                            ys[2]);

                            if (!attributeFilter.empty()) {
                                ogr_layer->SetAttributeFilter(attributeFilter.c_str());
                            }

                            // Loop through features.
                            OGRFeature *ogr_feature;
                            while ((ogr_feature = ogr_layer->GetNextFeature()) != nullptr) {

                                if (featurePainterSetupFunction != nullptr) {
                                    featurePainterSetupFunction(ogr_feature, painter);
                                } else {
                                    painter.setPen(getPen());
                                    painter.setBrush(getBrush());
                                }

                                // Draw the feature.
                                drawFeature(ogr_feature, painter, controller_zoom);

                                // Destroy the feature.
                                OGRFeature::DestroyFeature(ogr_feature);
                            }
                        }
                    }
                }
            }
        }
    }

    void ESRIShapefile::drawFeature(OGRFeature* ogr_feature, QPainter& painter, const int& controller_zoom) const
    {
        // Fetch geometries.
        const auto ogr_geometry(ogr_feature->GetGeometryRef());
        if(ogr_geometry == nullptr)
        {
            // No geometry to fetch!
        }
        // Is it a polygon.
        else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbPolygon)
        {
            // Cast to a polygon.
            const auto ogr_polygon(static_cast<OGRPolygon*>(ogr_geometry));

            // Fetch the exterior ring.
            const auto ogr_exterior_ring(ogr_polygon->getExteriorRing());
            if(ogr_exterior_ring == nullptr)
            {
                // No exterior ring!
            }
            else
            {
                QPainterPath path;

                // Prepare storage for point.
                OGRPoint ogr_point;

                // Create a polygon of the points.
                QPolygonF polygon_px;

                // Loop through the points.
                for(int i = 0; i < ogr_exterior_ring->getNumPoints(); ++i)
                {
                    // Fetch the point.
                    ogr_exterior_ring->getPoint(i, &ogr_point);
                    toWorldCoords(ogr_point);

                    // Add the point to be drawn.
                    polygon_px.append(projection::get().toPointWorldPx(PointWorldCoord(ogr_point.getX(), ogr_point.getY()), controller_zoom).rawPoint());
                }

                path.addPolygon(polygon_px);

                QPainterPath inp;
                for (int i = 0; i < ogr_polygon->getNumInteriorRings(); ++i) {
                    auto inn = ogr_polygon->getInteriorRing(i);
                    QPolygonF pf;
                    for (int j = 0; j < inn->getNumPoints(); ++j) {
                        inn->getPoint(j, &ogr_point);
                        toWorldCoords(ogr_point);
                        pf.append(projection::get().toPointWorldPx(PointWorldCoord(ogr_point.getX(), ogr_point.getY()),
                                                                   controller_zoom).rawPoint());
                    }
                    inp.addPolygon(pf);
                }

                path = path.subtracted(inp);

                // Draw the polygon line.
                painter.drawPath(path);
            }
        }
        else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbMultiPolygon)
        {
            // Cast to a multi polygon.
            const auto ogr_multi_polygon(static_cast<OGRMultiPolygon*>(ogr_geometry));
            if(ogr_multi_polygon == nullptr)
            {
                // No multi polygons!
            }
            else
            {
                QPainterPath path;

                // Loop through each polygon.
                for(int i = 0; i < ogr_multi_polygon->getNumGeometries(); ++i)
                {
                    // Cast to a polygon.
                    const auto ogr_polygon(static_cast<OGRPolygon*>(ogr_multi_polygon->getGeometryRef(i)));

                    // Fetch the exterior ring.
                    const auto ogr_exterior_ring(ogr_polygon->getExteriorRing());
                    if(ogr_exterior_ring == nullptr)
                    {
                        // No exterior ring!
                    }
                    else
                    {
                        // Prepare storage for point.
                        OGRPoint ogr_point;

                        // Create a polygon of the points.
                        QPolygonF polygon_px;

                        // Loop through the points.
                        for(int i = 0; i < ogr_exterior_ring->getNumPoints(); ++i)
                        {
                            // Fetch the point.
                            ogr_exterior_ring->getPoint(i, &ogr_point);
                            toWorldCoords(ogr_point);

                            // Add the point to be drawn.
                            polygon_px.append(projection::get().toPointWorldPx(PointWorldCoord(ogr_point.getX(), ogr_point.getY()), controller_zoom).rawPoint());
                        }

                        path.addPolygon(polygon_px);

                        QPainterPath inp;
                        for (int i = 0; i < ogr_polygon->getNumInteriorRings(); ++i) {
                            auto inn = ogr_polygon->getInteriorRing(i);
                            QPolygonF pf;
                            for (int j = 0; j < inn->getNumPoints(); ++j) {
                                inn->getPoint(j, &ogr_point);
                                toWorldCoords(ogr_point);
                                pf.append(projection::get().toPointWorldPx(
                                        PointWorldCoord(ogr_point.getX(), ogr_point.getY()),
                                        controller_zoom).rawPoint());
                            }
                            inp.addPolygon(pf);
                        }

                        path = path.subtracted(inp);

                    }
                }
                painter.drawPath(path);

            }
        }
        else if(wkbFlatten(ogr_geometry->getGeometryType()) == wkbLineString) // wkbLineString
        {
            // Cast to a line string.
            const auto ogr_line_string(static_cast<OGRLineString *>(ogr_geometry));

            // Prepare storage for point.
            OGRPoint ogr_point;

            // Create a polygon of the points.
            QPolygonF polygon_line_px;

            // Loop through the points.
            for (int i = 0; i < ogr_line_string->getNumPoints(); ++i) {
                // Fetch the point.
                ogr_line_string->getPoint(i, &ogr_point);
                toWorldCoords(ogr_point);

                // Add the point to be drawn.
                polygon_line_px.append(
                        projection::get().toPointWorldPx(PointWorldCoord(ogr_point.getX(), ogr_point.getY()),
                                                         controller_zoom).rawPoint());
            }

            // Draw the polygon line.
            painter.drawPolyline(polygon_line_px);
        } else if (wkbFlatten(ogr_geometry->getGeometryType()) == wkbPoint) {
            auto ogr_point(static_cast<OGRPoint *>(ogr_geometry));
            toWorldCoords(*ogr_point);
            auto point = projection::get().toPointWorldPx(PointWorldCoord(ogr_point->getX(), ogr_point->getY()),
                                                          controller_zoom).rawPoint().toPoint();

            QRect pointRect;
            pointRect.setSize(mPointGeometrySize);
            pointRect.moveCenter(point);

            painter.drawEllipse(pointRect);
        } else {
//            qDebug() << "Unsupported feature: " << ogr_geometry->getGeometryType();
        }
    }

std::vector<OGRFeature *> ESRIShapefile::findFeatureByRect(RectWorldCoord rw)
{
    std::vector<OGRFeature *> foundFeatures;

    auto points = rw.toStdVector();
    std::vector<double> xs, ys;
    std::transform(points.begin(), points.end(), std::back_inserter(xs),
                   [](PointWorldCoord const &pt) {
                       return pt.rawPoint().x();
                   });
    std::transform(points.begin(), points.end(), std::back_inserter(ys),
                   [](PointWorldCoord const &pt) {
                       return pt.rawPoint().y();
                   });

    // Note: sequence is: topleft, topright, botRIGHT, botleft
    mInvTransformation->Transform(points.size(), xs.data(), ys.data());

    auto minX = std::min(xs[0], xs[1]);
    auto maxX = std::max(xs[0], xs[1]);
    auto minY = std::min(ys[1], ys[2]);
    auto maxY = std::max(ys[1], ys[2]);

    for (int i = 0; i < m_ogr_data_set->GetLayerCount(); ++i) {
        const auto ogr_layer(m_ogr_data_set->GetLayer(i));

        ogr_layer->ResetReading();
        ogr_layer->SetSpatialFilterRect(minX,
                                        minY,
                                        maxX,
                                        maxY);

        if (!attributeFilter.empty()) {
            ogr_layer->SetAttributeFilter(attributeFilter.c_str());
        }

        OGRFeature *ogr_feature;
        while ((ogr_feature = ogr_layer->GetNextFeature()) != nullptr) {
            foundFeatures.push_back(ogr_feature);
        }
    }

    return foundFeatures;
}

void ESRIShapefile::toWorldCoords(OGRPoint &ogr) const
{
    double x = ogr.getX();
    double y = ogr.getY();

    mTransformation->Transform(1, &x, &y);
    ogr.setX(x);
    ogr.setY(y);
}

void ESRIShapefile::setAttributeFilter(std::string filter)
{
    attributeFilter = std::move(filter);
    emit requestRedraw();
}

void ESRIShapefile::clearAttributeFilter()
{
    attributeFilter.clear();
    emit requestRedraw();
}

void ESRIShapefile::setFeaturePainterSetupFunction(ESRIShapefile::FeaturePainterSetupFunction f)
{
    featurePainterSetupFunction = f;
}

}
