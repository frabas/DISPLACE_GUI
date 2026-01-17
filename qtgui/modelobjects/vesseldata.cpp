// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

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

QString VesselData::getName() const
{
    return mName;
}

void VesselData::setName(const QString &value)
{
    mName = value;
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

void VesselData::setDiscard(int idx, double val)
{
    while (mDiscards.size() < idx)
        mDiscards.push_back(0);
    mDiscards.push_back(val);
}


void VesselData::addDiscard(int idx, double val)
{
    while (mDiscards.size() <= idx)
        mDiscards.push_back(0);
    mDiscards[idx] += val;
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

void VesselData::addFuelCost(double value)
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


double VesselData::getSweptArea() const
{
    return sweptArea;
}

void VesselData::setSweptArea(double value)
{
    sweptArea = value;
}

void VesselData::addSweptArea(double value)
{
    sweptArea += value;
}

double VesselData::getRevenuePerSweptArea() const
{
    return revenuePerSweptArea;
}

void VesselData::setRevenuePerSweptArea(double value)
{
    revenuePerSweptArea = value;
}

void VesselData::addRevenuePerSweptArea(double value)
{
    revenuePerSweptArea = value;
}

void VesselData::addVpuf(double value)
{
    vpuf = value;
}


double VesselData::getGVA() const
{
    return GVA;
}

void VesselData::setGVA(double value)
{
    GVA = value;
}

void VesselData::addGVA(double value)
{
    GVA = value;
}

double VesselData::getGVAPerRevenue() const
{
    return GVAPerRevenue;
}

void VesselData::setGVAPerRevenue(double value)
{
    GVAPerRevenue = value;
}

void VesselData::addGVAPerRevenue(double value)
{
    GVAPerRevenue = value;
}

double VesselData::getLabourSurplus() const
{
    return LabourSurplus;
}

void VesselData::setLabourSurplus(double value)
{
    LabourSurplus = value;
}

void VesselData::addLabourSurplus(double value)
{
    LabourSurplus = value;
}

double VesselData::getGrossProfit() const
{
    return GrossProfit;
}

void VesselData::setGrossProfit(double value)
{
    GrossProfit = value;
}

void VesselData::addGrossProfit(double value)
{
    GrossProfit = value;
}

double VesselData::getNetProfit() const
{
    return NetProfit;
}

void VesselData::setNetProfit(double value)
{
    NetProfit = value;
}

void VesselData::addNetProfit(double value)
{
    NetProfit = value;
}

double VesselData::getNetProfitMargin() const
{
    return NetProfitMargin;
}

void VesselData::setNetProfitMargin(double value)
{
    NetProfitMargin = value;
}

void VesselData::addNetProfitMargin(double value)
{
    NetProfitMargin = value;
}

double VesselData::getGVAPerFTE() const
{
    return GVAPerFTE;
}

void VesselData::setGVAPerFTE(double value)
{
    GVAPerFTE = value;
}

void VesselData::addGVAPerFTE(double value)
{
    GVAPerFTE = value;
}

double VesselData::getRoFTA() const
{
    return RoFTA;
}

void VesselData::setRoFTA(double value)
{
    RoFTA = value;
}

void VesselData::addRoFTA(double value)
{
    RoFTA = value;
}

double VesselData::getBER() const
{
    return BER;
}

void VesselData::setBER(double value)
{
    BER = value;
}

void VesselData::addBER(double value)
{
    BER = value;
}

double VesselData::getCRBER() const
{
    return CRBER;
}

void VesselData::setCRBER(double value)
{
    CRBER = value;
}

void VesselData::addCRBER(double value)
{
    CRBER = value;
}

double VesselData::getNetPresentValue() const
{
    return NetPresentValue;
}

void VesselData::setNetPresentValue(double value)
{
    NetPresentValue = value;
}

void VesselData::addNetPresentValue(double value)
{
    NetPresentValue = value;
}

int VesselData::getNumTrips() const
{
    return numTrips;
}

void VesselData::setNumTrips(int value)
{
    numTrips = value;
}

void VesselData::addNumTrips(int value)
{
    numTrips = value;
}

void VesselData::setReasonToGoBack(int value)
{
    reasonToGoBack = value;
}
