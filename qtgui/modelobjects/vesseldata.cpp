#include "vesseldata.h"

VesselData::VesselData(std::shared_ptr<Vessel> vessel)
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

int VesselData::getLastHarbour() const
{
    return mLastHarbour;
}

void VesselData::setLastHarbour(int value)
{
    mLastHarbour = value;
}
double VesselData::getRevenue() const
{
    return mRevenue;
}

void VesselData::setRevenue(double value)
{
    mRevenue = value;
}
double VesselData::getRevenueAV() const
{
    return mRevenueAV;
}

void VesselData::setRevenueAV(double value)
{
    mRevenueAV = value;
}

void VesselData::setCatch(int idx, double val)
{
    while (mCatches.size() < idx)
        mCatches.push_back(0);
    mCatches.push_back(val);
}

void VesselData::addCatch(int idx, double val)
{
    while (mCatches.size() <= idx)
        mCatches.push_back(0);
    mCatches[idx] += val;
}
double VesselData::getCumFuelCons() const
{
    return cumFuelCons;
}

void VesselData::setCumFuelCons(double value)
{
    cumFuelCons = value;
}
double VesselData::getFuelCost() const
{
    return fuelCost;
}

void VesselData::setFuelCost(double value)
{
    fuelCost = value;
}
double VesselData::getGav() const
{
    return gav;
}

void VesselData::setGav(double value)
{
    gav = value;
}



