// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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
 * Copyright (c) 2012, 2013, 2014, 2015 Francois Bastardie <fba@aqua.dtu.dk>
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
#ifndef IPCMSGTYPES_H
#define IPCMSGTYPES_H

#include <stddef.h>

enum IpcMessageTypes {
    ForceExit,
    GenericConsole,
    VesselLogbook,
    ExportVms,
    MoveVessel,
    MoveShip
};

namespace displace {
namespace ipc {

struct MoveVesselMessage {
    unsigned int tstep, tstep_dep;
    unsigned int idx;
    double x,y,course,cumfuelcons;
    int state;
};

struct MoveShipMessage {
    unsigned int tstep;
    unsigned int idx;
    double x,y,course;
};

struct VesselLogbookMessage {
    unsigned int tstep, tstepdep;
    int rtbb, node, idx;
    double cumstm, timeatsea,cumfcons,travdist, revenue_from_av_prices, revenue_explicit_from_av_prices, fuelcost, gav2;
    size_t popnum;
    double pop[];
};

}
}

#endif // IPCMSGTYPES_H
