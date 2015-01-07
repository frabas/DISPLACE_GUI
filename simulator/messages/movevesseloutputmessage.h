#ifndef MOVEVESSELOUTPUTMESSAGE_H
#define MOVEVESSELOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <ipcmsgtypes.h>

class Vessel;

class MoveVesselOutputMessage : public OutputMessage
{
public:
    MoveVesselOutputMessage(int _tstep, Vessel *vessel);
    ~MoveVesselOutputMessage() {
    }

    virtual IpcMessageTypes getType() const {
        return MoveVessel;
    }

    bool process();
    bool send(std::ostream &strm);
    size_t sendBinary(void *buffer, size_t maxlen);

private:
    displace::ipc::MoveVesselMessage data;
};

#endif // MOVEVESSELOUTPUTMESSAGE_H
