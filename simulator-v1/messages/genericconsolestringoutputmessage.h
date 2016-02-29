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
#ifndef GENERICCONSOLESTRINGOUTPUTMESSAGE_H
#define GENERICCONSOLESTRINGOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <string>

class GenericConsoleStringOutputMessage : public OutputMessage
{
public:
    explicit GenericConsoleStringOutputMessage(const std::string &txt);

    virtual IpcMessageTypes getType() const {
        return GenericConsole;
    }

    bool process();
    bool send(std::ostream &strm);
    size_t sendBinary(void *buffer, size_t maxlen);

private:
    std::string msg;
};

#endif // GENERICCONSOLESTRINGOUTPUTMESSAGE_H
