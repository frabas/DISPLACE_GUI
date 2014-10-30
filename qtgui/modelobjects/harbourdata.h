#ifndef HARBOURDATA_H
#define HARBOURDATA_H

#include <Harbour.h>
#include <QVector>

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

    QVector<double> szCatches;
};

#endif // HARBOURDATA_H
