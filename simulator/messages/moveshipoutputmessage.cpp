#include "moveshipoutputmessage.h"
#include <mutexlocker.h>
#include <Ship.h>
#include <helpers.h>

extern pthread_mutex_t glob_mutex;

MoveShipOutputMessage::MoveShipOutputMessage(int _tstep, Ship *ship)
{
    data.tstep = _tstep;
    data.idx = ship->get_idx();
    data.x = ship->get_x();
    data.y = ship->get_y();
    data.course = ship->get_course();
 }

bool MoveShipOutputMessage::process()
{
    return true;
}

bool MoveShipOutputMessage::send(ostream &strm)
{
    MutexLocker locker(&glob_mutex);

    strm << "=X" << data.tstep << " "
        << data.idx << " "
        << setprecision(6) << fixed << data.x << " "
        << setprecision(6) << fixed << data.y << " "
        << setprecision(2) << fixed << data.course << endl;

    return true;
}

size_t MoveShipOutputMessage::sendBinary(void *buffer, size_t maxlen)
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
