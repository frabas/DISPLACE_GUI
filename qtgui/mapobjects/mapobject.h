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
#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <memory>

namespace qmapcontrol {
class Geometry;
}

class MapObject
{
public:
    MapObject();
    virtual ~MapObject();


    virtual std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const = 0;
    virtual bool showProperties();
    virtual void updateProperties();

    virtual void toggleSelection() final;
    virtual void setSelection(bool selection);

    /** Called when the selection has changed. Override this if you want specific action to be performed */
    virtual void onSelectionChanged();
};

#endif // MAPOBJECT_H
