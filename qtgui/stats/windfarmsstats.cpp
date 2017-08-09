#include "windfarmsstats.h"

#include <QDebug>

const WindfarmsStats::StatData WindfarmsStats::NoData;

WindfarmsStats::WindfarmsStats()
{

}

WindfarmsStats::WindfarmsStats(const WindfarmsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerWindFarmTypeAndWindFarmId = b.mDataPerWindFarmTypeAndWindFarmId;
}

WindfarmsStats::WindfarmsStats(WindfarmsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerWindFarmTypeAndWindFarmId = std::move(b.mDataPerWindFarmTypeAndWindFarmId);
}

WindfarmsStats &WindfarmsStats::operator =(const WindfarmsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerWindFarmTypeAndWindFarmId = b.mDataPerWindFarmTypeAndWindFarmId;

    return *this;
}

WindfarmsStats &WindfarmsStats::operator =(WindfarmsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerWindFarmTypeAndWindFarmId = std::move(b.mDataPerWindFarmTypeAndWindFarmId);

    return *this;
}

WindfarmsStats::StatData &WindfarmsStats::get(int windfarmid, int windfarmtype)
{
    std::unique_lock<std::mutex> l(mMutex);

    while (mDataPerWindFarmTypeAndWindFarmId.length() <= windfarmtype)
        mDataPerWindFarmTypeAndWindFarmId.push_back(QVector<StatData>());

    auto &v = mDataPerWindFarmTypeAndWindFarmId[windfarmtype];
    while (v.length() <= windfarmid)
        v.push_back(StatData());

    return v[windfarmid];
}

const WindfarmsStats::StatData &WindfarmsStats::get(int windfarmid, int windfarmtype) const
{
    std::unique_lock<std::mutex> l(mMutex);

    if (mDataPerWindFarmTypeAndWindFarmId.size() <= windfarmtype)
        return NoData;    // no data available, return empty data.

    auto &v = mDataPerWindFarmTypeAndWindFarmId[windfarmtype];
    if (v.size() <= windfarmid)
        return NoData;// no data available, return empty data.

    return v[windfarmid];
}


void WindfarmsStats::clear()
{
    std::unique_lock<std::mutex> l(mMutex);
    for (int i = 0; i < mDataPerWindFarmTypeAndWindFarmId.length(); ++i) {
        auto &v = mDataPerWindFarmTypeAndWindFarmId[i];
        for (int j = 0; j < v.length(); ++j)
            v.clear();
    }
    setDirty(false);
}


void WindfarmsStats::collectkWh(int step, int windfarmid, int windfarmtypeid, double kWh)
{
    Q_UNUSED(step);

    get(windfarmid, windfarmtypeid).kWh += kWh;
    setDirty();
}

void WindfarmsStats::collectkWproduction(int step, int windfarmid, int windfarmtypeid, double kWproduction)
{
    Q_UNUSED(step);

    get(windfarmid, windfarmtypeid).kWproduction += kWproduction;
    setDirty();
}

double WindfarmsStats::kWhForWindfarmAndWindfarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).kWh;
}

double WindfarmsStats::kWProductionForWindfarmAndWindfarmGroup(int farmid, int farmtypeid) const
{
    return get(farmid, farmtypeid).kWproduction;
}
