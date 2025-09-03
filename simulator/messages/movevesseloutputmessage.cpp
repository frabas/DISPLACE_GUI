#include "movevesseloutputmessage.h"
#include <mutex>
#include <Vessel.h>
#include <helpers.h>

extern std::mutex glob_mutex;

MoveVesselOutputMessage::MoveVesselOutputMessage(int _tstep, Vessel *vessel)
{
    data.tstep = _tstep;
    data.idx = vessel->get_idx();
    data.tstep_dep = vessel->get_tstep_dep();
    data.x = vessel->get_x();
    data.y = vessel->get_y();
    data.course = vessel->get_course();
    data.cumfuelcons = vessel->get_cumfuelcons();
    data.state = vessel->get_state();
}

bool MoveVesselOutputMessage::process()
{
    return true;
}

bool MoveVesselOutputMessage::send(ostream &strm)
{
    std::unique_lock<std::mutex> locker(glob_mutex);

    strm << "=V" << data.tstep << " "
        << data.idx << " "
        << data.tstep_dep << " "
        << setprecision(6) << fixed << data.x << " "
        << setprecision(6) << fixed << data.y << " "
        << setprecision(2) << fixed << data.course << " "
        << setprecision(0) << fixed << data.cumfuelcons << " "
        << data.state <<  "\n";

    return true;
}

size_t MoveVesselOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    UNUSED(maxlen);

    size_t i = 0;
    i = put(buffer, i, data);
    /*i = put(buffer, i, idx);
    i = put(buffer, i, tstep_dep);
    i = put(buffer, i, x);
    i = put(buffer, i, y);
    i = put(buffer, i, course);
    i = put(buffer, i, cumfuelcons);
    i = put(buffer, i, state); */
    return i;
}
