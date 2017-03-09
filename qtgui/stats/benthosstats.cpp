#include "benthosstats.h"

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
}

BenthosStats &BenthosStats::operator =(BenthosStats &&b)
{
    std::unique_lock<std::mutex> l1(mMutex);
    std::unique_lock<std::mutex> l2(b.mMutex);

    mDirty = std::move(b.mDirty);
    mDataPerBenthosAndFuncId = std::move(b.mDataPerBenthosAndFuncId);
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
    auto &v = mDataPerBenthosAndFuncId[benthos];
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

double BenthosStats::biomassForBenthosAndFuncGroup(int benthos, int funcgroup) const
{
    return get(funcgroup,benthos).biomass;
}

double BenthosStats::numberForBenthosAndFuncGroup(int benthos, int funcgroup) const
{
    return get(funcgroup,benthos).number;
}
