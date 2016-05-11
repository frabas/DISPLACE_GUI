#ifndef METIERDATA_H
#define METIERDATA_H

#include <QList>

class MetierData {
public:
    MetierData() {}
};

class MetierStats {
public:
    MetierStats() {}

    double revenueAV;
    double gav;
    double mTotCatches;

    QList<double> mCatches;
};

#endif // METIERDATA_H
