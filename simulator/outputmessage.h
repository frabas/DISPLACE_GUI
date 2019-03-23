// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include <ipcmsgtypes.h>
#include <ostream>

/** @brief an abstract class to serialize outputs through the OutputQueueManager
 *
 * */

class OutputMessage
{
public:
    OutputMessage();
    virtual ~OutputMessage() {
    }

    virtual IpcMessageTypes getType() const = 0;

    /** @brief perform an operation non related to the gui feedback channel
     * @return false if the calling function should exit. True otherwise.
     */
    virtual bool process() = 0;

    /** @brief the operation to perform.
     * @return false if error.
     * */
    virtual bool send(std::ostream &) = 0;

    /** @brief formats a binary block of data to be sent across the IPC stream
     * @arg buffer address of the data buffer
     * @arg maxlen the maximum size of the block
     * @return lenght of the block, or -1 if the process should exit (due to error, or other)
    */
    virtual size_t sendBinary(void *buffer, size_t maxlen) = 0;

    template <typename T>
    size_t put (void *buffer, size_t index, T v) {
        for (size_t i = 0; i < sizeof(T); ++i) {
            *((char *)buffer + index + i) = *((char *)&v + i);
        }
        return index + sizeof(T);
    }
};

#endif // OUTPUTMESSAGE_H
