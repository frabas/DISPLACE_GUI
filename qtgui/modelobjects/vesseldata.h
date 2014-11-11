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

    int getCatchesListSize() const { return mCatches.size(); }
    double getCatch(int idx) const { return mCatches.at(idx); }
    void setCatch(int idx, double val);
    void addCatch(int idx, double val);

    double getCumFuelCons() const;
    void setCumFuelCons(double value);

    double getFuelCost() const;
    void setFuelCost(double value);

    double getGav() const;
    void setGav(double value);

private:
    int mNationality;
    int mLastHarbour;
    double mRevenueAV;
    double cumFuelCons;
    double fuelCost;
    double gav;

    QList<double> mCatches;
};

class VesselStats {
public:
    double vpuf() const { return gav / cumFuelCons; }

    int tstep;

    int vesselId;
    int lastHarbour;
    double revenueAV;
    double timeAtSea;
    int reasonToGoBack;
    double cumFuelCons;
    double fuelCost;
    double gav;

    QList<double> mCatches;

};

#endif // VESSELDATA_H
