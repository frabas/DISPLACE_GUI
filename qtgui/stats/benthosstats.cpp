#include "benthosstats.h"


const BenthosStats::StatData BenthosStats::NoData;

BenthosStats::BenthosStats()
{

}

BenthosStats::BenthosStats(const BenthosStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerBenthosAndFuncId = b.mDataPerBenthosAndFuncId;
}

BenthosStats::BenthosStats(BenthosStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerBenthosAndFuncId = std::move(b.mDataPerBenthosAndFuncId);
}

BenthosStats &BenthosStats::operator =(const BenthosStats &b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = b.mDirty;
    mDataPerBenthosAndFuncId = b.mDataPerBenthosAndFuncId;

    return *this;
}

BenthosStats &BenthosStats::operator =(BenthosStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerBenthosAndFuncId = std::move(b.mDataPerBenthosAndFuncId);

    return *this;
}

BenthosStats::StatData &BenthosStats::get(int funcid, int benthos)
{
    std::unique_lock<std::mutex> l(mMutex);

    while (mDataPerBenthosAndFuncId.length() <= benthos)
        mDataPerBenthosAndFuncId.push_back(QVector<StatData>());

    auto &v = mDataPerBenthosAndFuncId[benthos];
    while (v.length() <= funcid)
        v.push_back(StatData());

    return v[funcid];
}

const BenthosStats::StatData &BenthosStats::get(int funcid, int benthos) const
{
    std::unique_lock<std::mutex> l(mMutex);

    if (mDataPerBenthosAndFuncId.size() <= benthos)
        return NoData;    // no data available, return empty data.

    auto &v = mDataPerBenthosAndFuncId[benthos];
    if (v.size() <= funcid)
        return NoData;// no data available, return empty data.

    return v[funcid];
}


void BenthosStats::clear()
{
    std::unique_lock<std::mutex> l(mMutex);
    for (int i = 0; i < mDataPerBenthosAndFuncId.length(); ++i) {
        auto &v = mDataPerBenthosAndFuncId[i];
        for (int j = 0; j < v.length(); ++j)
            v.clear();
    }
    setDirty(false);
}

void BenthosStats::collectBiomass(int step, int funcid, int benthosid, double biomass)
{
    Q_UNUSED(step);

    get(funcid,benthosid).biomass += biomass;
    setDirty();
}

void BenthosStats::collectNumber(int step, int funcid, int benthosid, double number)
{
    Q_UNUSED(step);

    get(funcid,benthosid).number += number;
    setDirty();
}

void BenthosStats::collectMeanWeight(int step, int funcid, int benthosid, double meanweight)
{
    Q_UNUSED(step);

    get(funcid,benthosid).meanweight += meanweight;
    setDirty();
}

void BenthosStats::collectBiomassOverK(int step, int funcid, int benthosid, double biomassOverK)
{
    Q_UNUSED(step);

    get(funcid,benthosid).biomassOverK = biomassOverK;
    setDirty();
}

void BenthosStats::collectNumberOverK(int step, int funcid, int benthosid, double numberOverK)
{
    Q_UNUSED(step);

    get(funcid,benthosid).numberOverK = numberOverK;
    setDirty();
}

double BenthosStats::biomassForBenthosAndFuncGroup(int funcgroup, int benthos) const
{
    return get(funcgroup,benthos).biomass;
}

double BenthosStats::numberForBenthosAndFuncGroup(int funcgroup, int benthos) const
{
    return get(funcgroup,benthos).number;
}

double BenthosStats::biomassOverKForBenthosAndFuncGroup(int funcgroup, int benthos) const
{
    return get(funcgroup,benthos).biomassOverK;
}

double BenthosStats::numberOverKForBenthosAndFuncGroup(int funcgroup, int benthos) const
{
    return get(funcgroup,benthos).numberOverK;
}
