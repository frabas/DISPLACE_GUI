#include "harbourdata.h"

HarbourData::HarbourData(std::shared_ptr<Harbour> harbour)
    : mHarbour(harbour)
{
}


HarbourStats::HarbourStats()
    : mCumCatches(0.0),
      mCumProfit(0.0),
      szCatches()
{
}
