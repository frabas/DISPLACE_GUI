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

    double getRevenue() const;
    void setRevenue(double value);

    double getRevenueAV() const;
    void setRevenueAV(double value);

    int getCatchesListSize() const { return mCatches.size(); }
    double getCatch(int idx) const { return mCatches.at(idx); }
    void setCatch(int idx, double val);
    void addCatch(int idx, double val);

private:
    int mNationality;
    int mLastHarbour;
    double mRevenue;
    double mRevenueAV;

    QList<double> mCatches;
};

class VesselStats {
public:
    int tstep;

    int vesselId;
    int lastHarbour;
    double revenue;
    double revenueAV;
    double timeAtSea;
    int reasonToGoBack;

    QList<double> mCatches;

};

#endif // VESSELDATA_H
