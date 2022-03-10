// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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


#ifndef IPCQUEUE_H
#define IPCQUEUE_H

#include <commons_global.h>
#include <ipcmsgtypes.h>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstddef>

/** @brief implements a CircularBuffer to be used in a boost::interprocess::managed_heap_memory
 * @note DO NOT implements ANY virtual method in this class! Pointers are not portable across processes!
 */
class  IpcQueue
{
public:
    IpcQueue();
    ~IpcQueue();

    bool push(IpcMessageTypes type, void *buffer, size_t len);
    IpcMessageTypes pickOrWait(void *buffer, size_t maxlen, size_t *len);

    static void forceCleanup();
private:
    bool empty() const;
    bool full() const;
    size_t space_available() const;
    bool push (char byte);
    char pop();

    struct MessageManager {
        boost::interprocess::interprocess_mutex mutex;
        boost::interprocess::interprocess_condition cond_notempty;
        boost::interprocess::interprocess_condition cond_notfull;

        static const size_t BufferSize = 512*1024;

        char buffer[BufferSize];
        int head;
        int tail;
        int size;

        MessageManager()
            : mutex(),
              cond_notempty(),
              cond_notfull(),
              head(0), tail(0),
              size(BufferSize)
        {
            
        }
    };

    static const size_t SharedMemorySize;
    static const char *SharedListName;

    boost::interprocess::shared_memory_object shmobj;
    boost::interprocess::mapped_region mapreg;

//    boost::interprocess::managed_shared_memory sharedMemory;
//    boost::interprocess::managed_heap_memory::handle_t sharedHandle;
    MessageManager *mManager;
};

#endif // IPCQUEUE_H
