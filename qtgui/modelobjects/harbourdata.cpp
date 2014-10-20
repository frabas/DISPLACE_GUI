#include "harbourdata.h"

HarbourData::HarbourData(Harbour *harbour)
    : mHarbour(harbour)
{
}


HarbourStats::HarbourStats()
    : mCumCatches(0.0),
      mCumProfit(0.0),
      szCatches()
{
}
