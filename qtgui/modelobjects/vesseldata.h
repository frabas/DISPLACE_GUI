#ifndef VESSELDATA_H
#define VESSELDATA_H

#include <Vessel.h>

class VesselData
{
    int mNationality;
public:
    Vessel *mVessel; /* This is crap too. */

    VesselData(Vessel *vessel = 0);

    int getNationality() const;
    void setNationality(int value);

private:
};

#endif // VESSELDATA_H
