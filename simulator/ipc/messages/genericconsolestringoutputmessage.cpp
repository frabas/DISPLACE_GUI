#include "genericconsolestringoutputmessage.h"

#include <mutexlocker.h>
#include <iostream>
#include <algorithm>

GenericConsoleStringOutputMessage::GenericConsoleStringOutputMessage(const std::string &txt)
    : msg(txt)
{
}

bool GenericConsoleStringOutputMessage::process()
{
    return true;
}

bool GenericConsoleStringOutputMessage::send(std::ostream &strm)
{
    strm << msg;

    return true;
}

size_t GenericConsoleStringOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    size_t len = std::min(maxlen, msg.size());

    for (size_t i = 0; i < len; ++i)
        *((char *)buffer + i) = msg.at(i);

    return len;
}
