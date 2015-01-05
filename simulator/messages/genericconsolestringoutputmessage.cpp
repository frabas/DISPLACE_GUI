#include "genericconsolestringoutputmessage.h"

#include <mutexlocker.h>
#include <iostream>
#include <algorithm>

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

int GenericConsoleStringOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    size_t len = std::min(maxlen, msg.size());

    for (size_t i = 0; i < len; ++i)
        *((char *)buffer + i) = msg.at(i);

    return len;
}
