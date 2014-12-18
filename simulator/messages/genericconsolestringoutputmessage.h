#ifndef GENERICCONSOLESTRINGOUTPUTMESSAGE_H
#define GENERICCONSOLESTRINGOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <string>

class GenericConsoleStringOutputMessage : public OutputMessage
{
public:
    explicit GenericConsoleStringOutputMessage(const std::string &txt);

    bool send();
private:
    std::string msg;
};

#endif // GENERICCONSOLESTRINGOUTPUTMESSAGE_H
