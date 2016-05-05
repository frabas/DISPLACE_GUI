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

#include <mutexlocker.h>
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
    pthread_mutex_init(&mMutex, 0);
    sem_init(&mSemaphore, 0, 0);
}

OutputQueueManager::OutputQueueManager(std::ostream &stream)
    : mType (TextWithStdOut),
      mOutStream (stream)
{
    pthread_mutex_init(&mMutex, 0);
    sem_init(&mSemaphore, 0, 0);
}

void OutputQueueManager::disableIpcQueue ()
{
    mType = None;
}

void OutputQueueManager::start()
{
    ThreadArgs *args = new ThreadArgs();

    args->obj = this;

    MutexLocker locker (&mMutex);
    UNUSED(locker);
    mThreadId = pthread_create(&mThread, 0, thread_trampoline, reinterpret_cast<void *>(args));
}

void OutputQueueManager::finish()
{
    enqueue(std::shared_ptr<OutputMessage>(new QuitMessage));

    void *out;
    pthread_join(mThread, &out);
}

void OutputQueueManager::enqueue(std::shared_ptr<OutputMessage> msg)
{
    MutexLocker locker(&mMutex);

    mQueue.push(msg);
    sem_post(&mSemaphore);
}

void *OutputQueueManager::thread_trampoline(void *args)
{
    ThreadArgs *arguments = reinterpret_cast<ThreadArgs *>(args);

    pthread_mutex_lock (&arguments->obj->mMutex);
    return arguments->obj->thread(arguments);
}

void *OutputQueueManager::thread(OutputQueueManager::ThreadArgs *args)
{
    UNUSED(args);

    pthread_mutex_unlock (&mMutex);

    bool exit = false;
    size_t len;
    char buffer [1024];

    while (!exit) {
        sem_wait(&mSemaphore);
        lock();
        if (mQueue.empty()) {
            unlock();
            continue;
        }
        std::shared_ptr<OutputMessage> msg = mQueue.front();
        mQueue.pop();
        unlock();

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
