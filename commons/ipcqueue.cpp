#include "ipcqueue.h"
#include <algorithm>
#include <boost/interprocess/sync/scoped_lock.hpp>

const size_t IpcQueue::SharedMemorySize = sizeof(MessageManager);    /* shared memory size */
const char* IpcQueue::SharedListName = "OutQueue";

IpcQueue::IpcQueue()
    : shmobj(boost::interprocess::open_or_create, "map", boost::interprocess::read_write)
//      mapreg(shmobj, boost::interprocess::read_write)
//    : sharedMemory(boost::interprocess::open_or_create, SharedListName, sizeof(MessageManager))
{
    shmobj.truncate(sizeof(MessageManager));
    mapreg = boost::interprocess::mapped_region(shmobj, boost::interprocess::read_write);
//    mManager = sharedMemory.find_or_construct<MessageManager>("out")();
    void *addr = mapreg.get_address();
    mManager = new (addr) IpcQueue::MessageManager;
}

bool IpcQueue::push(IpcMessageTypes type, void *buffer, size_t len)
{
    mManager->mutex.lock();

    for (size_t i = 0; i < sizeof(type); ++i) {
        push (*(((char *)&type) + i));
    }

    for (size_t i = 0; i < len; ++i) {
        push (*(((char *)&len) + i));
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

