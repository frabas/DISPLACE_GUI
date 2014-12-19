#ifndef GENERICCONSOLESTRINGOUTPUTMESSAGE_H
#define GENERICCONSOLESTRINGOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <string>

class GenericConsoleStringOutputMessage : public OutputMessage
{
public:
    explicit GenericConsoleStringOutputMessage(const std::string &txt);

    bool send(std::ostream &strm);
    bool sendBinary(std::ostream &);
private:
    std::string msg;
};

#endif // GENERICCONSOLESTRINGOUTPUTMESSAGE_H
