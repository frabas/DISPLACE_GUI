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

    double revenueAV;
    double gav;
    double mTotCatches;

    QList<double> mCatchesPerPop;
};

#endif // METIERDATA_H
