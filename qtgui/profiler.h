#ifndef PROFILER_H
#define PROFILER_H

#include <time.h>

#ifdef DEBUG
#ifndef PROFILE
#define PROFILE
#endif
#endif

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
        return 0.0;
#endif
    }

};

class AverageProfiler {
private:
#ifdef PROFILE
    struct timespec mstart;
    double mcumtime;
    int mcumrun;
#endif

public:
    AverageProfiler()
#ifdef PROFILE
        : mstart(), mcumtime(0.0), mcumrun(0)
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

    double total() {
#ifdef PROFILE
        return mcumtime;
#else
        return 0.0;
#endif
    }
};

#endif // PROFILER_H
