#ifndef MOVEVESSELOUTPUTMESSAGE_H
#define MOVEVESSELOUTPUTMESSAGE_H

#include <outputmessage.h>

class Vessel;

class MoveVesselOutputMessage : public OutputMessage
{
public:
    MoveVesselOutputMessage(int _tstep, Vessel *vessel);
    ~MoveVesselOutputMessage() {
    }

    virtual int getType() const {
        return 3;
    }

    bool send(std::ostream &strm);
    int sendBinary(void *buffer, int maxlen);

private:
    unsigned int tstep, tstep_dep;
    unsigned int idx;
    double x,y,course,cumfuelcons;
    int state;
};

#endif // MOVEVESSELOUTPUTMESSAGE_H
