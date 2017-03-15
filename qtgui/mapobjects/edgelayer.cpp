// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "edgelayer.h"

#include <mapobjects/edgemapobject.h>
#include <mapobjectscontroller.h>

EdgeLayer::EdgeLayer(MapObjectsController *controller, const QString &name, QObject *parent) :
    QObject(parent),
    mController(controller),
    mLayer(new qmapcontrol::LayerGeometry(name.toStdString()))
{
}

void EdgeLayer::addEdge(EdgeMapObject *edge, bool disable_redraw )
{
    mLayer->addGeometry(edge->getGeometryEntity(), disable_redraw);
}

void EdgeLayer::removeEdge(EdgeMapObject *edge)
{
    mLayer->removeGeometry(edge->getGeometryEntity());
}

void EdgeLayer::clear()
{
    mLayer->clearGeometries();
}
