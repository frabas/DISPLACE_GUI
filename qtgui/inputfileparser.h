// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <idtypes.h>
#include <memory>

#include <QList>
#include <QString>
#include <QPair>
#include <QSet>
#include <QMap>

#include <graphbuilder_shp.h>

class HarbourData;

class InputFileParser
{
public:
    InputFileParser();

    bool parseHarbourFile (const QString &path, QList<std::shared_ptr<HarbourData> > &list, QString *error);
    bool parseShortestPath (const QString &path, int nodeid, QList<QPair<int, int> > &graph);
    bool isShortestPathPresent (const QString &path, types::NodeId nodeid);

    bool parseGraph(const QString &graphpath, const QString &coordsPath, QList<GraphBuilder::Node> &nodes, QString *error);

    bool pathParseRelevantNodes (const QString &refpath, QString &fnodePath, QString &harbPath);
    bool pathParseRelevantInterNodes (const QString &refpath, QString &fnodePath, QString &harbPath);
    bool parseRelevantNodes (const QString &file, QSet<int> &nodes);
    bool parseRelevantInterNodes (const QString &file, QVector<int> &nodeids);

    bool parseStockNamesFile (const QString &path, QMap<QString,int> &names);
};

#endif // INPUTFILEPARSER_H
