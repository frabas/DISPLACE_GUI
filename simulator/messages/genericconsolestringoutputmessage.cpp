#include "genericconsolestringoutputmessage.h"

#include <mutexlocker.h>
#include <iostream>

extern pthread_mutex_t glob_mutex;

GenericConsoleStringOutputMessage::GenericConsoleStringOutputMessage(const std::string &txt)
    : msg(txt)
{
}

bool GenericConsoleStringOutputMessage::send(std::ostream &strm)
{
    MutexLocker l(&glob_mutex);
    strm << msg;

    return true;
}

bool GenericConsoleStringOutputMessage::sendBinary(std::ostream &)
{
    return true;
}
