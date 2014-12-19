#ifndef EXPORTVMSLIKEOUTPUTMESSAGE_H
#define EXPORTVMSLIKEOUTPUTMESSAGE_H

#include <outputmessage.h>

#include <Vessel.h>

#include <ostream>
#include <string>

class ExportVmslikeOutputMessage : public OutputMessage
{
public:
    ExportVmslikeOutputMessage(std::ostream &strm, unsigned int _tstep, Vessel *vessel);

    bool send();

private:
    std::ostream &vmslike;

    unsigned int tstep, tstep_dep;
    std::string name;
    double x,y,course,cumfuelcons;
    int state;
};

#endif // EXPORTVMSLIKEOUTPUTMESSAGE_H
