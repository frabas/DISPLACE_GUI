#include "ipcqueue.h"
#include <algorithm>
#include <boost/interprocess/sync/scoped_lock.hpp>

const size_t IpcQueue::SharedMemorySize = sizeof(MessageManager);    /* shared memory size */
const char* IpcQueue::SharedListName = "OutQueue";

IpcQueue::IpcQueue()
    : sharedMemory(sizeof(MessageManager))
{
    //sharedList = sharedMemory.construct<SharedList>(SharedListName)(sharedMemory.get_segment_manager());
    mManager = sharedMemory.find_or_construct<MessageManager>(SharedListName)();
    sharedHandle = sharedMemory.get_handle_from_address(mManager);
}

bool IpcQueue::push(IpcMessageTypes type, void *buffer, size_t len)
{
    mManager->mutex.lock();

    for (size_t i = 0; i < sizeof(type); ++i) {
        push (*(((char *)&type) + i));
        /*
        mManager->buffer[mManager->head] = *(((char *)&type) + i);
        ++mManager->head;
        if (mManager->head > mManager->size)
            mManager->head = 0;*/
    }

    for (size_t i = 0; i < len; ++i) {
        push (*(((char *)&len) + i));
        /*
        mManager->buffer[mManager->head] = ;
        ++mManager->head;
        if (mManager->head > mManager->size)
            mManager->head = 0;*/
    }

    for (size_t i = 0; i < len; ++i) {
        push(*(reinterpret_cast<char *>(buffer) + i));
    }

    mManager->cond.notify_one();
    mManager->mutex.unlock();

    return true;
}

IpcMessageTypes IpcQueue::pickOrWait(void *buffer, size_t maxlen, size_t *len)
{
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mManager->mutex);
    if (empty())
        mManager->cond.wait(lock);

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

    return type;
}

bool IpcQueue::empty() const
{
    return mManager->tail == mManager->head;
}

bool IpcQueue::full() const
{
    return ((mManager->tail +1) % mManager->size) == mManager->head;
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

