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
        return 0.0;
#endif
    }

};

#endif // PROFILER_H
