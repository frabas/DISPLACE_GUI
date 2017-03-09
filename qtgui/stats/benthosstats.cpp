#include "benthosstats.h"

BenthosStats::BenthosStats()
{

}

void BenthosStats::clear()
{
    for (int i = 0; i < mDataPerFuncId.length(); ++i) {
        mDataPerFuncId[i].clear();
    }
    setDirty(false);
}

void BenthosStats::collectBiomass(int step, int funcid, int node_idx, double biomass)
{
    Q_UNUSED(step);
    Q_UNUSED(node_idx);

    while (mDataPerFuncId.length() <= funcid)
        mDataPerFuncId.push_back(StatData());

    mDataPerFuncId[funcid].biomass += biomass;
    setDirty();
}

void BenthosStats::collectNumber(int step, int funcid, int node_idx, double number)
{
    Q_UNUSED(step);
    Q_UNUSED(node_idx);

    while (mDataPerFuncId.length() <= funcid)
        mDataPerFuncId.push_back(StatData());

    mDataPerFuncId[funcid].number += number;
    setDirty();
}
