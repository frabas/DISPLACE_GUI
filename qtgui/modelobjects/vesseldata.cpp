// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

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

double VesselData::getRevenueAV() const
{
    return mRevenueAV;
}

void VesselData::setRevenueAV(double value)
{
    mRevenueAV = value;
}

double VesselData::getRevenueExAV() const
{
    return mRevenueExAV;
}

void VesselData::setRevenueExAV(double value)
{
    mRevenueExAV = value;
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

void VesselData::addCumFuelCons(double value)
{
    cumFuelCons += value;
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



