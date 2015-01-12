#ifndef EXPORTVMSLIKEOUTPUTMESSAGE_H
#define EXPORTVMSLIKEOUTPUTMESSAGE_H

#include <outputmessage.h>

#include <Vessel.h>

#include <ostream>
#include <string>

/** @brief Output Vessel data to the vmslike file
 */
class ExportVmslikeOutputMessage : public OutputMessage
{
public:
    ExportVmslikeOutputMessage(ostream &strm, unsigned int _tstep, Vessel *vessel);

    virtual IpcMessageTypes getType() const {
        return ExportVms;
    }

    bool process();
    bool send(std::ostream &);
    size_t sendBinary(void *buffer, size_t maxlen);

private:
    std::ostream &vmslike;

    unsigned int tstep, tstep_dep;
    std::string name;
    double x,y,course,cumfuelcons;
    int state;
};

#endif // EXPORTVMSLIKEOUTPUTMESSAGE_H
