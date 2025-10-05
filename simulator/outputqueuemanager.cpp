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

#include "outputqueuemanager.h"

#include <outputmessage.h>

#include <helpers.h>

#include <iostream>

class QuitMessage : public OutputMessage {
public:
    QuitMessage() {
    }
    ~QuitMessage() {
    }

    virtual IpcMessageTypes getType() const {
        return ForceExit;
    }

    bool process() {
        return false;
    }

    bool send(std::ostream &) {
        return false;
    }

    size_t sendBinary(void *buffer, size_t maxlen){
        UNUSED(buffer);
        UNUSED(maxlen);

        /* ask for exit */
        return -1;
    }

};

OutputQueueManager::OutputQueueManager()
    : ipcQueue(),
      mType(Binary),
      mOutStream(std::cout)
{
}

OutputQueueManager::OutputQueueManager(std::ostream &stream)
    : mType (TextWithStdOut),
      mOutStream (stream)
{
}

void OutputQueueManager::disableIpcQueue ()
{
    mType = None;
}

void OutputQueueManager::start()
{
    std::unique_lock<std::mutex> locker(mMutex);
    UNUSED(locker);
    mThread = std::thread( [this]() { thread(); });
}

void OutputQueueManager::finish()
{
    enqueue(std::shared_ptr<OutputMessage>(new QuitMessage));

    if (mThread.joinable())
        mThread.join();
}

void OutputQueueManager::enqueue(std::shared_ptr<OutputMessage> msg)
{
    std::unique_lock<std::mutex> locker(mMutex);
    mQueue.push(msg);
    mCond.notify_all();
}

void OutputQueueManager::thread()
{
    bool exit = false;
    size_t len;
    char buffer [1024];

    while (!exit) {
        std::shared_ptr<OutputMessage> msg;
        {
            std::unique_lock<std::mutex> locker(mMutex);
            mCond.wait(locker, [&]() { return !mQueue.empty(); });

            msg = mQueue.front();
            mQueue.pop();
        }

        exit = !msg->process();

        if (!exit) {
            switch (mType) {
            case None:
                break;
            case TextWithStdOut:
                exit = !msg->send(mOutStream);
                break;
            case Binary:
                len = msg->sendBinary(buffer, 1024);
                if (len == (size_t)-1) {
                    exit = true;
                } else if (len > 0) {
                    IpcMessageTypes t = msg->getType();
                    //                std::cout << "** Sending message type " << t << " length: " << len << "\n";
                    ipcQueue.push(t,buffer, len);
                }
                break;
            }
        }
    }
}
