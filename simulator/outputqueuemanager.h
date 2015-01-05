#ifndef OUTPUQUEUEMANAGER_H
#define OUTPUQUEUEMANAGER_H

#include <pthread.h>
#include <semaphore.h>

#include <queue>
#include <list>
#include <memory>
#include <ostream>

#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstddef>

class OutputMessage;

/** @brief A manager for the outputs from all the other simulator threads. */
class OutputQueueManager
{
public:
    explicit OutputQueueManager();
    explicit OutputQueueManager(std::ostream &stream);

    void start();
    void finish();

    void lock() {
        pthread_mutex_lock(&mMutex);
    }

    void unlock() {
        pthread_mutex_unlock(&mMutex);
    }

    void enqueue(std::shared_ptr<OutputMessage> msg);
private:
    /** @brief The pthread-style entry point to the thread. */
    static void *thread_trampoline(void *);

    struct ThreadArgs {
        OutputQueueManager *obj;
    };

    void *thread(ThreadArgs *args);


    /* == members == */

    int mThreadId;
    pthread_t mThread;
    pthread_mutex_t mMutex;
    sem_t mSemaphore;

    std::queue<std::shared_ptr<OutputMessage> > mQueue;

    /* == shared structure == */

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

    enum ProtocolType { TextWithStdOut, Binary } mType;

    std::ostream &mOutStream;        ///< File descriptor for socket/pipe (Binary protocol)
};

#endif // OUTPUQUEUEMANAGER_H
