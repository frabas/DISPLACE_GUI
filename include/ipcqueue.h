#ifndef IPCQUEUE_H
#define IPCQUEUE_H

#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstddef>

class IpcQueue
{
public:
    IpcQueue();

    bool push(int type, void *buffer, size_t len);
private:
    struct MessageManager {
        boost::interprocess::interprocess_mutex mutex;
        boost::interprocess::interprocess_condition cond;

        char buffer[16*1024*1024];
        int head;
        int tail;
        int size;

        MessageManager()
            : mutex(),
              cond(),
              head(0), tail(0),
              size(sizeof(buffer))
        {
        }
    };

    static const size_t SharedMemorySize;
    static const char *SharedListName;

    boost::interprocess::managed_heap_memory sharedMemory;
    boost::interprocess::managed_heap_memory::handle_t sharedHandle;
    MessageManager *mManager;
};

#endif // IPCQUEUE_H
