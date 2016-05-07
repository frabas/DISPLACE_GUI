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
#ifndef VESSELLOGBOOKOUTPUTMESSAGE_H
#define VESSELLOGBOOKOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <ipcmsgtypes.h>

#include <ostream>
#include <string>
#include <vector>

class Vessel;
class Population;

class VesselLogbookOutputMessage : public OutputMessage
{
public:
    VesselLogbookOutputMessage(unsigned int _tstep, Vessel *v, const std::vector<Population *> &populations, std::vector <int> &implicit_pops);

    virtual IpcMessageTypes getType() const {
        return VesselLogbook;
    }

    bool process();
    bool send(std::ostream &);
    virtual size_t sendBinary(void *buffer, size_t maxlen);

private:
    std::string name, freq_metiers;
    int length_class;

    std::vector<double> cumul;
    std::vector<double> cumul_discards;

    displace::ipc::VesselLogbookMessage logbook;
};

#endif // VESSELLOGBOOKOUTPUTMESSAGE_H
