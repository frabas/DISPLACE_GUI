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
};

#endif // HARBOURDATA_H
