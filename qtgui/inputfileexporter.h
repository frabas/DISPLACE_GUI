// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef INPUTFILEEXPORTER_H
#define INPUTFILEEXPORTER_H

#include <QString>

#include <vector>
#include <functional>

class DisplaceModel;

namespace displace {
class NodePenalty;
}

QT_BEGIN_NAMESPACE
class QTextStream;
QT_END_NAMESPACE

class InputFileExporter
{
public:
    InputFileExporter();

    bool exportGraph(QString graphpath, QString coordspath, QString landpath,  QString windpath, QString sstpath, QString salinitypath,
                     QString Nitrogenpath, QString Phosporuspath, QString Oxygenpath, QString DissolvedCarbonpath, QString bathymetrypath, QString shippingdensitypath,
                     QString siltfractionpath, QString icesrectanglecodepath, QString benthospath, QString benthosnbpath, QString areacodepath,
                     QString closedpath, QString closedpath_month, QString closedpath_vessz, bool export_closedpoly, DisplaceModel *currentModel, QString *error);
protected:
    bool outputClosedPolyFile(QString filename, DisplaceModel *currentModel,
                              std::function<bool(const displace::NodePenalty &)> selector,
                              std::function<std::vector<int>(const displace::NodePenalty &)> dataGetter,
                              QString *error);
};

#endif // INPUTFILEEXPORTER_H
