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
#ifndef EDGELAYER_H
#define EDGELAYER_H

#include <QObject>

#include <QMapControl/LayerGeometry.h>
#include <mapobjectscontroller.h>

class EdgeMapObject;

class EdgeLayer : public QObject
{
    Q_OBJECT
public:
    explicit EdgeLayer(MapObjectsController *controller, const QString &name, QObject *parent = 0);

    void setVisible(bool vi) { mLayer->setVisible(vi); }
    void addEdge (EdgeMapObject *edge, bool disable_redraw = false);
    void removeEdge (EdgeMapObject *edge);
    void clear();

    std::shared_ptr<qmapcontrol::LayerGeometry> layer() const { return mLayer; }

    const QList<EdgeMapObject *>selection() const { return mSelection; }

private:
    MapObjectsController *mController;
    std::shared_ptr<qmapcontrol::LayerGeometry> mLayer;

    QList<EdgeMapObject *> mSelection;
};

#endif // EDGELAYER_H
