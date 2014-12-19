#include "genericconsolestringoutputmessage.h"

#include <mutexlocker.h>
#include <iostream>

extern pthread_mutex_t glob_mutex;

GenericConsoleStringOutputMessage::GenericConsoleStringOutputMessage(const std::string &txt)
    : msg(txt)
{
}

bool GenericConsoleStringOutputMessage::send()
{
    MutexLocker l(&glob_mutex);
    std::cout << msg;

    return true;
}
