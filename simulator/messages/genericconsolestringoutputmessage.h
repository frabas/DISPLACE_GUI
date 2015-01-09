#ifndef GENERICCONSOLESTRINGOUTPUTMESSAGE_H
#define GENERICCONSOLESTRINGOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <string>

class GenericConsoleStringOutputMessage : public OutputMessage
{
public:
    explicit GenericConsoleStringOutputMessage(const std::string &txt);

    virtual IpcMessageTypes getType() const {
        return GenericConsole;
    }

    bool process();
    bool send(std::ostream &strm);
    size_t sendBinary(void *buffer, size_t maxlen);

private:
    std::string msg;
};

#endif // GENERICCONSOLESTRINGOUTPUTMESSAGE_H
