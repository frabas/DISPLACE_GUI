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
    ThreadArgs *args = new ThreadArgs();

    args->obj = this;

    std::unique_lock<std::mutex> locker(mMutex);
    UNUSED(locker);
    mThread = std::move(std::thread([args]() {
        thread_trampoline(args);
    }));
}

void OutputQueueManager::finish()
{
    enqueue(std::shared_ptr<OutputMessage>(new QuitMessage));

    if (mThread.joinable())
        mThread.join();
}

void OutputQueueManager::enqueue(std::shared_ptr<OutputMessage> msg)
{
    std::unique_lock<std::mutex>(mMutex);
    mQueue.push(msg);
    mCond.notify_one();
}

void *OutputQueueManager::thread_trampoline(void *args)
{
    ThreadArgs *arguments = reinterpret_cast<ThreadArgs *>(args);

    arguments->obj->mMutex.lock();
    return arguments->obj->thread(arguments);
}

void *OutputQueueManager::thread(OutputQueueManager::ThreadArgs *args)
{
    UNUSED(args);

    mMutex.unlock();

    bool exit = false;
    size_t len;
    char buffer [1024];

    while (!exit) {

        std::unique_lock<std::mutex> locker(mMutex);
        while (mQueue.empty())
            mCond.wait(locker);

        std::shared_ptr<OutputMessage> msg = mQueue.front();
        mQueue.pop();
        locker.unlock();

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
                    //                std::cout << "** Sending message type " << t << " length: " << len << std::endl;
                    ipcQueue.push(t,buffer, len);
                }
                break;
            }
        }
    }

    return 0;
}
