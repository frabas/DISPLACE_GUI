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

#include <commons_global.h>
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

struct COMMONSSHARED_EXPORT MoveVesselMessage {
    unsigned int tstep = 0, tstep_dep = 0;
    unsigned int idx = -1;
    double x = 0,y = 0,course = 0,cumfuelcons = 0;
    int state = -1;
};

struct COMMONSSHARED_EXPORT MoveShipMessage {
    unsigned int tstep = 0;
    unsigned int idx = -1;
    double x = 0,y = 0,course = 0;
};

struct COMMONSSHARED_EXPORT VesselLogbookMessage {
    unsigned int tstep = 0, tstepdep = 0;
    int rtbb = 0, node = -1, idx = -1;
    double cumstm = 0, timeatsea = 0,cumfcons = 0,travdist = 0, revenue_from_av_prices = 0, revenue_explicit_from_av_prices = 0, fuelcost = 0, gav2 = 0;
    size_t popnum = 0;
    int metier = 0;
    double pop[];
};

}
}

#endif // IPCMSGTYPES_H
