#include "ipcqueue.h"

const size_t IpcQueue::SharedMemorySize = sizeof(MessageManager);    /* shared memory size */
const char* IpcQueue::SharedListName = "OutQueue";

IpcQueue::IpcQueue()
    : sharedMemory(sizeof(MessageManager))
{
    //sharedList = sharedMemory.construct<SharedList>(SharedListName)(sharedMemory.get_segment_manager());
    mManager = sharedMemory.construct<MessageManager>(SharedListName)();
    sharedHandle = sharedMemory.get_handle_from_address(mManager);
}

bool IpcQueue::push(int type, void *buffer, size_t len)
{
    mManager->mutex.lock();

    for (size_t i = 0; i < sizeof(type); ++i) {
        mManager->buffer[mManager->head] = *(((char *)&type) + i);
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
        mManager->buffer[mManager->head] = *(reinterpret_cast<char *>(buffer) + i);
        ++mManager->head;
        if (mManager->head > mManager->size)
            mManager->head = 0;
    }

    mManager->cond.notify_one();
    mManager->mutex.unlock();

    return true;
}

