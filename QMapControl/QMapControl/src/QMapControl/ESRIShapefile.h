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

#pragma once

// Qt includes.
#include <QtCore/QObject>
#include <QtGui/QBrush>
#include <QtGui/QPainter>
#include <QtGui/QPen>

// GDAL includes.
#include <ogrsf_frmts.h>

// STL includes.
#include <memory>
#include <string>
#include <functional>

// Local includes.
#include "qmapcontrol_global.h"
#include "Point.h"

namespace qmapcontrol
{
    //! ESRIShapefile class
    /*!
     * Object to present an ESRI Shapefile.
     *
     * @author Chris Stylianou <chris5287@gmail.com>
     */
    class QMAPCONTROL_EXPORT ESRIShapefile : public QObject
    {
        Q_OBJECT
    public:
        //! ESRIShapefile constructor
        /*!
         * This is used to construct a ESRIShapefile.
         * @param file_path The file path of the ESRI Shapefile.
         * @param layer_name The name of the layer.
         * @param zoom_minimum The minimum zoom level to show this ESRI Shapefile at.
         * @param zoom_maximum The maximum zoom level to show this ESRI Shapefile at.
         */
        explicit ESRIShapefile(const std::string &file_path, const std::string &layer_name, const int &zoom_minimum = 0,
                               const int &zoom_maximum = 17);

        explicit ESRIShapefile(GDALDataset *datasource, const std::string &layer_name, const int &zoom_minimum = 0,
                               const int &zoom_maximum = 17);

        //! Destructor.
        virtual ~ESRIShapefile();

        /**
         * @brief Prevents the dataset from being destroyed when the object is freed.
         * If another object is destroying the dataset, this would result in a double free error.
         * This prevents the error from happening.
         */
        void releaseDatasetOwnership()
        {
            hasDatasetOwnership = false;
        }

        /*!
         * Fetches the pen to draw the polygon with (outline).
         * @return the QPen to used for drawing.
         */
        const QPen &getPen() const;

        /*!
         * Sets the pen to draw the polygon with (outline).
         * @param pen The QPen to used for drawing.
         */
        void setPen(QPen pen);

        /*!
         * Fetches the brush to draw the polygon with (fill).
         * @return the QBrush to used for drawing.
         */
        const QBrush &getBrush() const;

        /*!
         * Sets the brush to draw the polygon with (fill).
         * @param brush The QBrush to used for drawing.
         */
        void setBrush(QBrush brush);

        /*!
         * Draws ESRI Shapefiles to a pixmap using the provided painter.
         * @param painter The painter that will draw to the pixmap.
         * @param backbuffer_rect_px Only draw geometries that are contained in the backbuffer rect (pixels).
         * @param controller_zoom The current controller zoom.
         */
        void draw(QPainter &painter, const RectWorldPx &backbuffer_rect_px, const int &controller_zoom) const;

        void setAttributeFilter(std::string filter);

        void clearAttributeFilter();

        using FeaturePainterSetupFunction = std::function<void(OGRFeature *, QPainter &)>;

        void setFeaturePainterSetupFunction(FeaturePainterSetupFunction function);

        /**
         * @brief Set the dimension of the Points geometries
         * @param sz Size of the Circle/Ellipse representing a Point Geometry
         */
        void setPointGeometrySize(QSize sz)
        {
            mPointGeometrySize = sz;
            requestRedraw();
        }

        QSize getPointGeometrySize() const
        {
            return mPointGeometrySize;
        }

        std::vector<OGRFeature *> findFeatureByRect(RectWorldCoord);

    protected:

        /*!
         * Draws ESRI Shapefile feature to a pixmap using the provided painter.
         * @param ogr_feature The feature to draw.
         * @param painter The painter that will draw to the pixmap.
         * @param controller_zoom The current controller zoom.
         */
        virtual void drawFeature(OGRFeature *ogr_feature, QPainter &painter, const int &controller_zoom) const;

        void createProjections(OGRSpatialReference *) const;

        void toWorldCoords(OGRPoint &ogr) const;

    signals:

        /*!
         * Signal emitted when a change has occurred that requires the layer to be redrawn.
         */
        void requestRedraw() const;

    private:
        /// The OGR data set of the ESRI Shapefile.
        GDALDataset *m_ogr_data_set;
        bool hasDatasetOwnership = true;

        /// The layer name.
        std::string m_layer_name;

        /// Minimum zoom level to show this ESRI Shapefile.
        int m_zoom_minimum;

        /// Maximum zoom level to show this ESRI Shapefile.
        int m_zoom_maximum;

        /// The pen to use when drawing a polygon.
        QPen mPen;

        /// The brush to use when drawing a polygon.
        QBrush mBrush;

        QSize mPointGeometrySize = QSize{10, 10};

        std::string attributeFilter;

        FeaturePainterSetupFunction featurePainterSetupFunction = nullptr;
    };
}
