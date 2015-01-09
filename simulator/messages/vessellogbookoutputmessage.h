#ifndef VESSELLOGBOOKOUTPUTMESSAGE_H
#define VESSELLOGBOOKOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <ipcmsgtypes.h>

#include <ostream>
#include <string>
#include <vector>

class Vessel;
class Population;

class VesselLogbookOutputMessage : public OutputMessage
{
public:
    VesselLogbookOutputMessage(std::ostream &strm, unsigned int _tstep, Vessel *v, const std::vector<Population *> &populations);

    virtual IpcMessageTypes getType() const {
        return VesselLogbook;
    }

    bool process();
    bool send(std::ostream &);
    virtual size_t sendBinary(void *buffer, size_t maxlen);

private:
    std::ostream &loglike;

    displace::ipc::VesselLogbookMessage logbook;
    std::string name, freq_metiers, length_class;

    std::vector<double> cumul;
};

#endif // VESSELLOGBOOKOUTPUTMESSAGE_H
