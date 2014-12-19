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

    bool send();

private:
    unsigned int tstep, tstep_dep;
    unsigned int idx;
    double x,y,course,cumfuelcons;
    int state;
};

#endif // MOVEVESSELOUTPUTMESSAGE_H
