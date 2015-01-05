#ifndef GENERICCONSOLESTRINGOUTPUTMESSAGE_H
#define GENERICCONSOLESTRINGOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <string>

class GenericConsoleStringOutputMessage : public OutputMessage
{
public:
    explicit GenericConsoleStringOutputMessage(const std::string &txt);

    virtual int getType() const {
        return 2;
    }

    bool send(std::ostream &strm);
    int sendBinary(void *buffer, int maxlen);

private:
    std::string msg;
};

#endif // GENERICCONSOLESTRINGOUTPUTMESSAGE_H
