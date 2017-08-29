#ifndef METIERDATA_H
#define METIERDATA_H

#include <QList>

class MetierData {
    int mId = 0;
public:
    MetierData() {}

    int getId() const { return mId; }
};

class MetierStats {
public:
    MetierStats() {}

    int numTrips = 0;

    double revenueAV = 0.0;
    double gav = 0.0;
    double mTotCatches = 0.0;
    double mTotDiscards = 0.0;
    double vpuf = 0.0;
    double mSweptArea = 0.0;
    double mRevenuePerSweptArea = 0.0;


    QList<double> mCatchesPerPop;
    QList<double> mDiscardsPerPop;

    double GVA = 0.0;
    double cumGVAPerRevenue = 0.0;
    double GVAPerRevenue = 0.0;
    double LabourSurplus = 0.0;
    double GrossProfit = 0.0;
    double NetProfit = 0.0;
    double NetProfitMargin = 0.0;
    double RoFTA = 0.0;
    double GVAPerFTE = 0.0;
    double BER = 0.0;
    double CRBER = 0.0;
    double NetPresentValue = 0.0;

};

#endif // METIERDATA_H
