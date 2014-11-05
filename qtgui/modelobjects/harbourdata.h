#ifndef HARBOURDATA_H
#define HARBOURDATA_H

#include <Harbour.h>
#include <QVector>

#include <memory>

class HarbourData
{
public:
    std::shared_ptr<Harbour> mHarbour;

    explicit HarbourData(std::shared_ptr<Harbour> harbour);
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
