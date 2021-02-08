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

struct  MoveVesselMessage {
    unsigned int tstep = 0, tstep_dep = 0;
    unsigned int idx = -1;
    double x = 0,y = 0,course = 0,cumfuelcons = 0;
    int state = -1;
};

struct  MoveShipMessage {
    unsigned int tstep = 0;
    unsigned int idx = -1;
    double x = 0,y = 0,course = 0;
};

struct  VesselLogbookMessage {
    unsigned int tstep = 0, tstepdep = 0;
    int rtbb = 0, node = -1, idx = -1;
    double count= 1;
    double cumstm = 0, timeatsea = 0,cumfcons = 0,travdist = 0;
    double revenue_from_av_prices = 0, revenue_explicit_from_av_prices = 0, fuelcost = 0, vpuf = 0, gav2 = 0, sweptarea=0, revenuepersweptarea=0;
    double GVA= 0;
    double GVAPerRevenue= 0;
    double LabourSurplus= 0;
    double GrossProfit= 0;
    double NetProfit= 0;
    double NetProfitMargin= 0;
    double GVAPerFTE= 0;
    double RoFTA= 0;
    double BER= 0;
    double CRBER= 0;
    double NetPresentValue= 0;
    int numTrips=0;
    size_t popnum = 0;
    int metier = 0;
    double pop[];
};

}
}

#endif // IPCMSGTYPES_H
