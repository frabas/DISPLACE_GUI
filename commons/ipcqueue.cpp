// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "ipcqueue.h"
#include <algorithm>
#include <boost/interprocess/sync/scoped_lock.hpp>

const size_t IpcQueue::SharedMemorySize = sizeof(MessageManager);    /* shared memory size */
const char *IpcQueue::SharedListName = "OutQueue";

using namespace boost::interprocess;

IpcQueue::IpcQueue()
{
    bool first = false;
    try {
        shmobj = boost::interprocess::shared_memory_object(boost::interprocess::open_only, SharedListName, read_write);
    } catch (interprocess_exception x) {
        // not found, create it!
        first = true;
    }

    if (first) {
        shmobj = shared_memory_object(create_only, SharedListName, read_write);
        shmobj.truncate(sizeof(MessageManager));
        mapreg = boost::interprocess::mapped_region(shmobj, boost::interprocess::read_write);
        void *addr = mapreg.get_address();
        mManager = new (addr) IpcQueue::MessageManager;
    } else {
        // shmobj alrady there
        mapreg = boost::interprocess::mapped_region(shmobj, boost::interprocess::read_write);
        void *addr = mapreg.get_address();
        mManager = reinterpret_cast<IpcQueue::MessageManager *>(addr);
    }
}

IpcQueue::~IpcQueue()
{
    shmobj.remove(SharedListName);
}

bool IpcQueue::push(IpcMessageTypes type, void *buffer, size_t len)
{
    scoped_lock<interprocess_mutex> lock (mManager->mutex);
    //    std::cout << "Push (Available: " << space_available() << " needed: " << (len + sizeof(type) + sizeof(len)) << ")" << "\n";
    while (space_available() < len + sizeof(type) + sizeof(len)) {
        //        std::cout<<"Wait for space"<< "\n";
        mManager->cond_notfull.wait(lock);
        //        std::cout << "Push (Available: " << space_available() << " needed: " << (len + sizeof(type) + sizeof(len)) << ")" << "\n";
    }

    for (size_t i = 0; i < sizeof(type); ++i) {
        push (*(((char *)&type) + i));
    }

    for (size_t i = 0; i < sizeof(len); ++i) {
        push (*(((char *)&len) + i));
    }

    for (size_t i = 0; i < len; ++i) {
        push(*(reinterpret_cast<char *>(buffer) + i));
    }

    //    std::cout << "At end Available: " << space_available() << "\n";
    mManager->cond_notempty.notify_one();

    return true;
}

IpcMessageTypes IpcQueue::pickOrWait(void *buffer, size_t maxlen, size_t *len)
{
    //    std::cout << "pick" << "\n";

    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mManager->mutex);
    if (empty()) {
        //        std::cout << "Wait,data" << "\n";
        mManager->cond_notempty.wait(lock);
        //        std::cout << "Got data" << "\n";
    }

    IpcMessageTypes type;
    for (size_t i = 0; i < sizeof(type); ++i) {
        *((char *)&type + i) = pop();
    }
    for (size_t i = 0; i < sizeof(size_t); ++i) {
        *((char *)len + i) = pop();
    }

    *len = std::min(maxlen, *len);
    for (size_t i = 0; i < *len; ++i) {
        *(reinterpret_cast<char *>(buffer) + i) = pop();
    }

    mManager->cond_notfull.notify_one();

    return type;
}

void IpcQueue::forceCleanup()
{
    bool found = true;
    while (found) {
        try {
            boost::interprocess::shared_memory_object shmobj;
            shmobj = boost::interprocess::shared_memory_object(boost::interprocess::open_only, SharedListName, read_write);
            shared_memory_object::remove(SharedListName);
        } catch (interprocess_exception) {
            found = false;
        }
    }
}

bool IpcQueue::empty() const
{
    return mManager->tail == mManager->head;
}

bool IpcQueue::full() const
{
    return ((mManager->tail +1) % mManager->size) == mManager->head;
}

size_t IpcQueue::space_available() const
{
    return (mManager->head + mManager->size - mManager->tail -1) % mManager->size;
}

bool IpcQueue::push(char byte)
{
    mManager->buffer[mManager->head] = byte;
    ++mManager->head;
    if (mManager->head >= mManager->size)
        mManager->head = 0;

    return true;
}

char IpcQueue::pop()
{
    char x = mManager->buffer[mManager->tail];
    ++mManager->tail;
    if (mManager->tail >= mManager->size)
        mManager->tail = 0;
    return x;
}

