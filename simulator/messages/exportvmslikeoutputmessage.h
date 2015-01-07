#ifndef EXPORTVMSLIKEOUTPUTMESSAGE_H
#define EXPORTVMSLIKEOUTPUTMESSAGE_H

#include <outputmessage.h>

#include <Vessel.h>

#include <ostream>
#include <string>

/** @brief an helper function to output data to the vmslike file
 *
 * This class shouldn't be used in the ipc queue, because it ignores the ipc paradigm.
 * It ignore the format used, so in the prospective of roles-responsibilities, it should be
 * used in another thread, not in the IPC thread.
 * It is used here as an heritage from the previous ipc mechanism that used streams (cout)
 * instead of the boost::interprocess method.
 *
 * @todo Reorganize this to be used in a separate thread.
 * */
class ExportVmslikeOutputMessage : public OutputMessage
{
public:
    ExportVmslikeOutputMessage(ostream &strm, unsigned int _tstep, Vessel *vessel);

    virtual IpcMessageTypes getType() const {
        return ExportVms;
    }

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
