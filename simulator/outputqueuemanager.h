#ifndef OUTPUQUEUEMANAGER_H
#define OUTPUQUEUEMANAGER_H

#include <pthread.h>
#include <semaphore.h>

#include <queue>
#include <memory>
#include <ostream>

class OutputMessage;

/** @brief A manager for the outputs from all the other simulator threads. */
class OutputQueueManager
{
public:
    explicit OutputQueueManager(std::ostream &stream, bool binary);

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

    enum ProtocolType { TextWithStdOut, Binary } mType;

    std::ostream &mOutStream;        ///< File descriptor for socket/pipe (Binary protocol)
};

#endif // OUTPUQUEUEMANAGER_H
