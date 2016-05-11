/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef VESSELDATA_H
#define VESSELDATA_H

#include <Vessel.h>
#include <QList>

#include <memory>

class VesselStats;

class VesselData
{
public:
    std::shared_ptr<Vessel> mVessel; /* This is crap too. */

    VesselData(std::shared_ptr<Vessel> vessel);

    int getNationality() const;
    void setNationality(int value);

    int getLastHarbour() const;
    void setLastHarbour(int value);

    double getRevenueAV() const;
    void setRevenueAV(double value);
    double getRevenueExAV() const;
    void setRevenueExAV(double value);

    int getCatchesListSize() const { return mCatches.size(); }
    double getCatch(int idx) const { return mCatches.at(idx); }
    void setCatch(int idx, double val);
    void addCatch(int idx, double val);

    double getCumFuelCons() const;
    void setCumFuelCons(double value);
    void addCumFuelCons(double value);

    double getFuelCost() const;
    void setFuelCost(double value);

    double getGav() const;
    void setGav(double value);

private:
    int mNationality;
    int mLastHarbour;
    double mRevenueAV;
    double mRevenueExAV;
    double cumFuelCons;
    double fuelCost;
    double gav;

    QList<double> mCatches;
};

class VesselStats {
public:
    double vpuf() const { return revenueAV / cumFuelCons; }

    int tstep;

    int vesselId;
    int metierId;
    int lastHarbour;
    double revenueAV;
    double revenueExAV;
    double timeAtSea;
    int reasonToGoBack;
    double cumFuelCons;
    double fuelCost;
    double gav;

    QList<double> mCatches;

};

#endif // VESSELDATA_H
