#ifndef MUTEXLOCKER_H
#define MUTEXLOCKER_H

#include <pthread.h>

/** @brief An auxiliary class to automatically manage mutexes.
 *
 * When created, the locker automatically locks the mutex.
 * When destroyed, it automatically unlocks it.
 * Typical usage:
 * @verbatim

 int x() {
    MutexLocker locker(&mutex);
    //....

    return 0;
    // mutex is unlocked on destroy.
 }

 */
class MutexLocker
{
public:
    explicit MutexLocker(pthread_mutex_t *mutex, bool locked = true)
        : mMutex(mutex) {
        if (locked)
            pthread_mutex_lock(mMutex);
    }

    ~MutexLocker() {
        pthread_mutex_unlock(mMutex);
    }

    void lock() {
        pthread_mutex_lock(mMutex);
    }

    void unlock() {
        pthread_mutex_unlock(mMutex);
    }

private:
    pthread_mutex_t *mMutex;
};

#endif // MUTEXLOCKER_H
