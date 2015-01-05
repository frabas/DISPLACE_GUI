#include "movevesseloutputmessage.h"
#include <mutexlocker.h>
#include <Vessel.h>

extern pthread_mutex_t glob_mutex;

MoveVesselOutputMessage::MoveVesselOutputMessage(int _tstep, Vessel *vessel)
{
    tstep = _tstep;
    idx = vessel->get_idx();
    tstep_dep = vessel->get_tstep_dep();
    x = vessel->get_x();
    y = vessel->get_y();
    course = vessel->get_course();
    cumfuelcons = vessel->get_cumfuelcons();
    state = vessel->get_state();
}

bool MoveVesselOutputMessage::send(ostream &strm)
{
    MutexLocker locker(&glob_mutex);

    strm << "=V" << tstep << " "
        << idx << " "
        << tstep_dep << " "
        << setprecision(6) << fixed << x << " "
        << setprecision(6) << fixed << y << " "
        << setprecision(2) << fixed << course << " "
        << setprecision(0) << fixed << cumfuelcons << " "
        << state <<  endl;

    return true;
}

int MoveVesselOutputMessage::sendBinary(void *buffer, int maxlen)
{
    return 0;
}
