/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014, 2015 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
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
