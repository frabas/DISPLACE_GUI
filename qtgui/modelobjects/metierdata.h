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

    double revenueAV = 0.0;
    double gav = 0.0;
    double mTotCatches = 0.0;
    double vpuf = 0.0;
    double mSweptArea = 0.0;
    double mRevenuePerSweptArea = 0.0;


    QList<double> mCatchesPerPop;
};

#endif // METIERDATA_H
