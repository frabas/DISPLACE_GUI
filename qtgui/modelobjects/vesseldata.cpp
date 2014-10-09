#include "vesseldata.h"

VesselData::VesselData(Vessel *vessel)
    : mVessel(vessel)
{
}

int VesselData::getNationality() const
{
    return mNationality;
}

void VesselData::setNationality(int value)
{
    mNationality = value;
}
