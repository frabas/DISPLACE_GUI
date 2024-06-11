// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef OUTPUQUEUEMANAGER_H
#define OUTPUQUEUEMANAGER_H

#include <ipcqueue.h>

#include <thread>
#include <condition_variable>

#include <queue>
#include <list>
#include <memory>
#include <ostream>

#include <memory>

class OutputMessage;

/** @brief A manager for the outputs from all the other simulator threads. */
class OutputQueueManager
{
public:
    explicit OutputQueueManager();
    explicit OutputQueueManager(std::ostream &stream);

    void disableIpcQueue();
    void start();
    void finish();

    void enqueue(std::shared_ptr<OutputMessage> msg);

private:
    void thread();


    /* == members == */

    std::thread mThread;
    std::mutex mMutex;
    std::condition_variable mCond;

    std::queue<std::shared_ptr<OutputMessage> > mQueue;

    IpcQueue ipcQueue;
    enum ProtocolType { TextWithStdOut, Binary, None } mType;

    std::ostream &mOutStream;        ///< File descriptor for socket/pipe (Binary protocol)
};

#endif // OUTPUQUEUEMANAGER_H
