/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
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
#ifndef PROFILER_H
#define PROFILER_H

#include <time.h>

class Profiler {
private:
#ifdef PROFILE
    struct timespec mstart;
#endif

public:
    Profiler() {
#ifdef PROFILE
        clock_gettime(CLOCK_REALTIME, &mstart);
#endif
    }

    double elapsed_ms(bool cumulative = false) {
#ifdef PROFILE
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        double frm = (double)mstart.tv_sec + (double)mstart.tv_nsec * 1e-9;
        double t = (double)now.tv_sec + (double)now.tv_nsec * 1e-9;

        if (!cumulative)
            mstart = now;
        return t-frm;
#else
        (void)cumulative;
        return 0.0;
#endif
    }

    void start() {
#ifdef PROFILE
        clock_gettime(CLOCK_REALTIME, &mstart);
#endif
    }

};

class AverageProfiler {
private:
#ifdef PROFILE
    struct timespec mstart;
    double mcumtime, mMin, mMax;
    int mcumrun;
#else
    static constexpr double mMin = 0.0, mMax = 0.0;
#endif

public:
    AverageProfiler()
#ifdef PROFILE
        : mstart(), mcumtime(0.0), mMin(0), mMax(0), mcumrun(0)
#endif
    {
#ifdef PROFILE
        clock_gettime(CLOCK_REALTIME, &mstart);
#endif
    }

    double elapsed_ms() {
#ifdef PROFILE
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        double frm = (double)mstart.tv_sec + (double)mstart.tv_nsec * 1e-9;
        double t = (double)now.tv_sec + (double)now.tv_nsec * 1e-9 - frm;
        mstart = now;
        mcumtime += t;
        if (mcumrun == 0) {
            mMin = t;
            mMax = t;
        } else {
            mMin = std::min(mMin,t);
            mMax = std::max(mMax,t);
        }

        ++mcumrun;
        return t;
#else
        return 0.0;
#endif
    }

    void start() {
#ifdef PROFILE
        clock_gettime(CLOCK_REALTIME, &mstart);
#endif
    }

    int runs() {
#ifdef PROFILE
        return mcumrun;
#else
        return 0;
#endif
    }

    double avg() {
#ifdef PROFILE
        return mcumtime / (double)mcumrun;
#else
        return 0.0;
#endif
    }

    double min() const { return mMin; }
    double max() const { return mMax; }

    double total() {
#ifdef PROFILE
        return mcumtime;
#else
        return 0.0;
#endif
    }
};

#endif // PROFILER_H
