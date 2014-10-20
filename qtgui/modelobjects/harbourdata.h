#ifndef HARBOURDATA_H
#define HARBOURDATA_H

#include <Harbour.h>

class HarbourData
{
public:
    Harbour *mHarbour;

    explicit HarbourData(Harbour *harbour);
};

class HarbourStats
{
public:
    HarbourStats();

    double mCumCatches;
    double mCumProfit;
};

#endif // HARBOURDATA_H
