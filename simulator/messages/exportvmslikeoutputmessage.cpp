#include "exportvmslikeoutputmessage.h"

#include <mutexlocker.h>

extern pthread_mutex_t glob_mutex;

ExportVmslikeOutputMessage::ExportVmslikeOutputMessage(ostream &strm, unsigned int _tstep, Vessel *vessel)
    : OutputMessage(),
      vmslike(strm),
      tstep(_tstep)
{
    name = vessel->get_name();
    tstep_dep = vessel->get_tstep_dep();
    x = vessel->get_x();
    y = vessel->get_y();
    course = vessel->get_course();
    cumfuelcons = vessel->get_cumfuelcons();
    state = vessel->get_state();
}

bool ExportVmslikeOutputMessage::send(std::ostream &)
{
    MutexLocker l(&glob_mutex);

    vmslike << tstep << " "
               //<< vessels[ index_v ]->get_idx() << " "
            << name << " "
               // can be used as a trip identifier
            << tstep_dep << " "
            << setprecision(3) << fixed << x << " "
            << setprecision(3) << fixed << y << " "
            << setprecision(0) << fixed << course << " "
               //<< vessels[ index_v ]->get_inharbour() << " "
            << setprecision(0) << fixed << cumfuelcons << " "
            << state << " " <<  endl;

    return true;
}

int ExportVmslikeOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    return 0;
}
