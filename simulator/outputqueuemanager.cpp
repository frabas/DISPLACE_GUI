#include "outputqueuemanager.h"

#include <outputmessage.h>

#include <mutexlocker.h>
#include <helpers.h>

OutputQueueManager::OutputQueueManager()
{
    pthread_mutex_init(&mMutex, 0);
    sem_init(&mSemaphore, 0, 0);
}

void OutputQueueManager::start()
{
    ThreadArgs *args = new ThreadArgs();

    args->obj = this;

    MutexLocker locker (&mMutex);
    UNUSED(locker);
    mThreadId = pthread_create(&mThread, 0, thread_trampoline, reinterpret_cast<void *>(args));
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
    while (!exit) {
        sem_wait(&mSemaphore);
        lock();
        if (mQueue.empty()) {
            unlock();
            continue;
        }
        std::shared_ptr<OutputMessage> msg = mQueue.front();
        exit = !msg->send();
        unlock();
    }

    return 0;
}
