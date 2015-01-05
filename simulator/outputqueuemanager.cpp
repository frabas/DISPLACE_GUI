#include "outputqueuemanager.h"

#include <outputmessage.h>

#include <mutexlocker.h>
#include <helpers.h>

#include <iostream>

const size_t OutputQueueManager::SharedMemorySize = sizeof(MessageManager);    /* shared memory size */
const char* OutputQueueManager::SharedListName = "OutQueue";

class QuitMessage : public OutputMessage {
public:
    QuitMessage() {
    }
    ~QuitMessage() {
    }

    virtual int getType() const {
        return -128;
    }

    bool send(std::ostream &) {
        return false;
    }

    int sendBinary(void *buffer, int maxlen){
        UNUSED(buffer);
        UNUSED(maxlen);

        /* ask for exit */
        return -1;
    }

};

OutputQueueManager::OutputQueueManager()
    : sharedMemory(SharedMemorySize),
      mType(Binary),
      mOutStream(std::cout)
{
    //sharedList = sharedMemory.construct<SharedList>(SharedListName)(sharedMemory.get_segment_manager());
    mManager = sharedMemory.construct<MessageManager>(SharedListName)();
    sharedHandle = sharedMemory.get_handle_from_address(mManager);

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

        switch (mType) {
        case TextWithStdOut:
            exit = !msg->send(mOutStream);
            break;
        case Binary:
            len = msg->sendBinary(buffer, 1024);
            if (len == (size_t)-1) {
                exit = true;
            } else if (len > 0) {
                mManager->mutex.lock();

                int t = msg->getType();
                for (size_t i = 0; i < sizeof(t); ++i) {
                    mManager->buffer[mManager->head] = *(((char *)&t) + i);
                    ++mManager->head;
                    if (mManager->head > mManager->size)
                        mManager->head = 0;
                }

                for (size_t i = 0; i < len; ++i) {
                    mManager->buffer[mManager->head] = *(((char *)&len) + i);
                    ++mManager->head;
                    if (mManager->head > mManager->size)
                        mManager->head = 0;
                }

                for (size_t i = 0; i < len; ++i) {
                    mManager->buffer[mManager->head] = *(buffer + i);
                    ++mManager->head;
                    if (mManager->head > mManager->size)
                        mManager->head = 0;
                }

                mManager->cond.notify_one();
                mManager->mutex.unlock();
            }
            break;
        }
    }

    return 0;
}
