#include "shipsstats.h"

#include <QDebug>

const ShipsStats::StatData ShipsStats::NoData;

ShipsStats::ShipsStats()
{

}

ShipsStats::ShipsStats(const ShipsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerShipTypeAndShipId = b.mDataPerShipTypeAndShipId;
}

ShipsStats::ShipsStats(ShipsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerShipTypeAndShipId = std::move(b.mDataPerShipTypeAndShipId);
}

ShipsStats &ShipsStats::operator =(const ShipsStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerShipTypeAndShipId = b.mDataPerShipTypeAndShipId;

    return *this;
}

ShipsStats &ShipsStats::operator =(ShipsStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerShipTypeAndShipId = std::move(b.mDataPerShipTypeAndShipId);

    return *this;
}

ShipsStats::StatData &ShipsStats::get(int shipid, int shiptype)
{
    std::unique_lock<std::mutex> l(mMutex);

    while (mDataPerShipTypeAndShipId.length() <= shiptype)
        mDataPerShipTypeAndShipId.push_back(QVector<StatData>());

    auto &v = mDataPerShipTypeAndShipId[shiptype];
    while (v.length() <= shipid)
        v.push_back(StatData());

    return v[shipid];
}

const ShipsStats::StatData &ShipsStats::get(int shipid, int shiptype) const
{
    std::unique_lock<std::mutex> l(mMutex);

    if (mDataPerShipTypeAndShipId.size() <= shiptype)
        return NoData;    // no data available, return empty data.

    auto &v = mDataPerShipTypeAndShipId[shiptype];
    if (v.size() <= shipid)
        return NoData;// no data available, return empty data.

    return v[shipid];
}


void ShipsStats::clear()
{
    std::unique_lock<std::mutex> l(mMutex);
    for (int i = 0; i < mDataPerShipTypeAndShipId.length(); ++i) {
        auto &v = mDataPerShipTypeAndShipId[i];
        for (int j = 0; j < v.length(); ++j)
            v.clear();
    }
    setDirty(false);
}


void ShipsStats::collectNbTransportedUnits(int step, int shipid, int shiptypeid, double nb_transported_units)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).nb_transported_units += nb_transported_units;
    setDirty();
}

void ShipsStats::collectFuelUseHour(int step, int shipid, int shiptypeid, double fuel_use_h)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).fuel_use_h += fuel_use_h;
    setDirty();
}

void ShipsStats::collectNOxEmissiongPerkW(int step, int shipid, int shiptypeid, double NOx_emission_gperkW)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).NOx_emission_gperkW += NOx_emission_gperkW;
    setDirty();
}

void ShipsStats::collectSOxEmissionPercentPerTotalFuelmass(int step, int shipid, int shiptypeid, double SOx_emission_percentpertotalfuelmass)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).SOx_emission_percentpertotalfuelmass += SOx_emission_percentpertotalfuelmass;
    setDirty();
}

void ShipsStats::collectGHGemission(int step, int shipid, int shiptypeid, double GHG_emission_gperkW)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).GHG_emission_gperkW += GHG_emission_gperkW;
    setDirty();
}

void ShipsStats::collectPMEemission(int step, int shipid, int shiptypeid, double PME_emission_gperkW)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).PME_emission_gperkW += PME_emission_gperkW;
    setDirty();
}

void ShipsStats::collectfuelUseLitre(int step, int shipid, int shiptypeid, double fuel_use_litre)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).fuel_use_litre += fuel_use_litre;
    setDirty();
}

void ShipsStats::collectNOxEmission(int step, int shipid, int shiptypeid, double NOx_emission)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).NOx_emission += NOx_emission;
    setDirty();
}

void ShipsStats::collectSOxEmission(int step, int shipid, int shiptypeid, double SOx_emission)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).SOx_emission += SOx_emission;
    setDirty();
}

void ShipsStats::collectGHGemissiongPerkW(int step, int shipid, int shiptypeid, double GHG_emission)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).GHG_emission += GHG_emission;
    setDirty();
}

void ShipsStats::collectPMEemissiongPerkW(int step, int shipid, int shiptypeid, double PME_emission)
{
    Q_UNUSED(step);

    get(shipid, shiptypeid).PME_emission += PME_emission;
    setDirty();
}


double ShipsStats::NbTransportedUnitsForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).nb_transported_units;
}


double ShipsStats::FuelPerHourForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).nb_transported_units;
}

double ShipsStats::NOxEmissionFactorForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).NOx_emission_gperkW;
}

double ShipsStats::SOxEmissionFactorForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).SOx_emission_percentpertotalfuelmass;
}

double ShipsStats::GHGEmissionFactorForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).GHG_emission_gperkW;
}

double ShipsStats::PMEEmissionFactorForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).PME_emission_gperkW;
}

double ShipsStats::FuelUseLitreForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).fuel_use_litre;
}

double ShipsStats::NOxEmissionForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).NOx_emission;
}

double ShipsStats::SOxEmissionForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).SOx_emission;
}

double ShipsStats::GHGEmissionForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).GHG_emission;
}

double ShipsStats::PMEEmissionForShipAndShipGroup(int shipid, int shiptypeid) const
{
    return get(shipid, shiptypeid).PME_emission;
}




