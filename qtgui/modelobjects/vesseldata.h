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
     QString mName;

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

    int getDiscardsListSize() const { return mDiscards.size(); }
    double getDiscard(int idx) const { return mDiscards.at(idx); }
    void setDiscard(int idx, double val);
    void addDiscard(int idx, double val);

    double getCumFuelCons() const;
    void setCumFuelCons(double value);
    void addCumFuelCons(double value);

    double getSweptArea() const;
    void setSweptArea(double value);
    void addSweptArea(double value);
    double getRevenuePerSweptArea() const;
    void setRevenuePerSweptArea(double value);
    void addRevenuePerSweptArea(double value);

    double getGVA() const;
    void setGVA(double value);
    void addGVA(double value);
    double getGVAPerRevenue() const;
    void setGVAPerRevenue(double value);
    void addGVAPerRevenue(double value);
    double getLabourSurplus() const;
    void setLabourSurplus(double value);
    void addLabourSurplus(double value);
    double getGrossProfit() const;
    void setGrossProfit(double value);
    void addGrossProfit(double value);
    double getNetProfit() const;
    void setNetProfit(double value);
    void addNetProfit(double value);
    double getNetProfitMargin() const;
    void setNetProfitMargin(double value);
    void addNetProfitMargin(double value);
    double getGVAPerFTE() const;
    void setGVAPerFTE(double value);
    void addGVAPerFTE(double value);
    double getRoFTA() const;
    void setRoFTA(double value);
    void addRoFTA(double value);
    double getBER() const;
    void setBER(double value);
    void addBER(double value);
    double getCRBER() const;
    void setCRBER(double value);
    void addCRBER(double value);
    double getNetPresentValue() const;
    void setNetPresentValue(double value);
    void addNetPresentValue(double value);
    int getNumTrips() const;
    void setNumTrips(int value);
    void addNumTrips(int value);
    void setReasonToGoBack(int value);



    double getFuelCost() const;
    void setFuelCost(double value);
    void addFuelCost(double value);

    void addVpuf(double value);

    double getGav() const;
    void setGav(double value);

    QString getName() const;
    void setName(const QString &value);

private:
    int mNationality;
    int mLastHarbour;
    double mRevenueAV;
    double mRevenueExAV;
    double cumFuelCons;
    double fuelCost;
    double vpuf;
    double gav;
    double sweptArea;
    double revenuePerSweptArea;

    double GVA;
    double GVAPerRevenue;
    double LabourSurplus;
    double GrossProfit;
    double NetProfit;
    double NetProfitMargin;
    double GVAPerFTE;
    double RoFTA;
    double BER;
    double CRBER;
    double NetPresentValue;
    int numTrips;
    int reasonToGoBack;


    QList<double> mCatches;
    QList<double> mDiscards;
};

class VesselStats {
public:
    //deprecated: double vpuf() const { return revenueAV / cumFuelCons; }

    int tstep = 0;

    int vesselId = -1;
    std::string name;
    int metierId = -1;
    int lastHarbour = -1;
    double revenueAV = 0;
    double revenueExAV = 0;
    double timeAtSea = 0;
    int reasonToGoBack = 0;
    double cumFuelCons = 0;
    double vpuf = 0;
    double fuelCost = 0;
    double gav = 0;
    double sweptArea = 0;
    double revenuePerSweptArea = 0;

    double GVA = 0;
    double GVAPerRevenue = 0;
    double LabourSurplus = 0;
    double GrossProfit = 0;
    double NetProfit = 0;
    double NetProfitMargin = 0;
    double RoFTA = 0;
    double GVAPerFTE = 0;
    double BER = 0;
    double CRBER = 0;
    double NetPresentValue = 0;
    int numTrips=1;
    

    QList<double> mCatches;
    QList<double> mDiscards;

};

#endif // VESSELDATA_H
