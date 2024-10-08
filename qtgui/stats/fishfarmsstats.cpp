#include "fishfarmsstats.h"

#include <QDebug>

const FishfarmsStats::StatData FishfarmsStats::NoData;

FishfarmsStats::FishfarmsStats()
{

}

FishfarmsStats::FishfarmsStats(const FishfarmsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerFarmTypeAndFarmId = b.mDataPerFarmTypeAndFarmId;
}

FishfarmsStats::FishfarmsStats(FishfarmsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerFarmTypeAndFarmId = std::move(b.mDataPerFarmTypeAndFarmId);
}

FishfarmsStats &FishfarmsStats::operator =(const FishfarmsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerFarmTypeAndFarmId = b.mDataPerFarmTypeAndFarmId;

    return *this;
}

FishfarmsStats &FishfarmsStats::operator =(FishfarmsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerFarmTypeAndFarmId = std::move(b.mDataPerFarmTypeAndFarmId);

    return *this;
}

FishfarmsStats::StatData &FishfarmsStats::get(int farmid, int farmtype)
{
    std::unique_lock<std::mutex> l(mMutex);

    while (mDataPerFarmTypeAndFarmId.length() <= farmtype)
        mDataPerFarmTypeAndFarmId.push_back(QVector<StatData>());

    auto &v = mDataPerFarmTypeAndFarmId[farmtype];
    while (v.length() <= farmid)
        v.push_back(StatData());

    return v[farmid];
}

const FishfarmsStats::StatData &FishfarmsStats::get(int farmid, int farmtype) const
{
    std::unique_lock<std::mutex> l(mMutex);

    if (mDataPerFarmTypeAndFarmId.size() <= farmtype)
        return NoData;    // no data available, return empty data.

    auto &v = mDataPerFarmTypeAndFarmId[farmtype];
    if (v.size() <= farmid)
        return NoData;// no data available, return empty data.

    return v[farmid];
}


void FishfarmsStats::clear()
{
    std::unique_lock<std::mutex> l(mMutex);
    for (int i = 0; i < mDataPerFarmTypeAndFarmId.length(); ++i) {
        auto &v = mDataPerFarmTypeAndFarmId[i];
        for (int j = 0; j < v.length(); ++j)
            v.clear();
    }
    setDirty(false);
}


void FishfarmsStats::collectMeanWeightKg(int step, int farmid, int farmtypeid, double meanw_kg)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).meanw += meanw_kg;
    setDirty();
}

void FishfarmsStats::collectFishHarvestedKg(int step, int farmid, int farmtypeid, double fish_harvested_kg)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).fishharvestkg += fish_harvested_kg;
    setDirty();
}

void FishfarmsStats::collectEggsHarvestedKg(int step, int farmid, int farmtypeid, double eggs_harvested_kg)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).eggsharvestkg += eggs_harvested_kg;
    setDirty();
}

void FishfarmsStats::collectFishfarmAnnualProfit(int step, int farmid, int farmtypeid, double fishfarm_annual_profit)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).annualprofit += fishfarm_annual_profit;
    setDirty();
}

void FishfarmsStats::collectNetDischargeN(int step, int farmid, int farmtypeid, double net_discharge_N)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).netdischargeN += net_discharge_N;
    setDirty();
}

void FishfarmsStats::collectNetDischargeP(int step, int farmid, int farmtypeid, double net_discharge_P)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).netdischargeP += net_discharge_P;
    setDirty();
}

void FishfarmsStats::collectCumulNetDischargeN(int step, int farmid, int farmtypeid, double cumul_net_discharge_N)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).cumulnetdischargeN += cumul_net_discharge_N;
    setDirty();
}

void FishfarmsStats::collectCumulNetDischargeP(int step, int farmid, int farmtypeid, double cumul_net_discharge_P)
{
    Q_UNUSED(step);

    get(farmid, farmtypeid).cumulnetdischargeP += cumul_net_discharge_P;
    setDirty();
}


double FishfarmsStats::meanwForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).meanw;
}

double FishfarmsStats::fishharvestedkgForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).fishharvestkg;
}

double FishfarmsStats::eggsharvestedkgForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).eggsharvestkg;
}

double FishfarmsStats::annualprofitForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).annualprofit;
}

double FishfarmsStats::netdischargeNForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).netdischargeN;
}

double FishfarmsStats::netdischargePForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).netdischargeP;
}

double FishfarmsStats::cumulnetdischargeNForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).cumulnetdischargeN;
}

double FishfarmsStats::cumulnetdischargePForFishfarmAndFarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).cumulnetdischargeP;
}
