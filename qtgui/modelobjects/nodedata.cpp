#include "nodedata.h"

#include <displacemodel.h>

NodeData::NodeData(std::shared_ptr<Node> nd, DisplaceModel *model)
    : mNode (nd),
      mModel(model),
      mDeleted(false),
      mPop(0),
      mPopTot(0),
      mPopW(0),
      mPopWTot(0),
      mHarbourId(-1)
{
    if (nd) {
        int N = nd->get_nbpops();
        mPop = new double[N] ;
        mPopW = new double[N] ;
        mImpact = new double[N];
        for (int i = 0; i < N; ++i) {
            mPop[i] = 0.0;
            mPopW[i] = 0.0;
            mImpact[i] = 0.0;
        }
    }
}

NodeData::~NodeData()
{
    delete []mPop;
    delete []mPopW;
    delete []mImpact;
}

int NodeData::getPopCount() const
{
    return mNode->get_nbpops();
}

void NodeData::setPop(int pop, double v)
{
    if (pop < mNode->get_nbpops() && pop > 0)
        mPop[pop] = v;
}

void NodeData::setPopTot(double tot)
{
    mPopTot = tot;
}

void NodeData::setPop(QList<double> v, double tot)
{
    for (int i=0; i < v.size() && i < mNode->get_nbpops(); ++i) {
        mPop[i] = v[i];
    }
    mPopTot = tot;
}

double NodeData::getPop(int pop) const
{
    if (pop < mNode->get_nbpops() && pop >= 0)
        return mPop[pop];

    return -1;
}

void NodeData::setPopW(int pop, double val)
{
    if (pop < mNode->get_nbpops() && pop > 0)
        mPopW[pop] = val;
}

void NodeData::setPopWTot(double tot)
{
    mPopWTot = tot;
}

void NodeData::setPopW(QList<double> v, double tot)
{
    for (int i=0; i < v.size() && i < mNode->get_nbpops(); ++i) {
        mPopW[i] = v[i];
    }
    mPopWTot = tot;
}

double NodeData::getPopW(int pop) const
{
    if (pop < mNode->get_nbpops() && pop >= 0)
        return mPopW[pop];

    return -1;
}

void NodeData::setImpact(int pop, double impact)
{
    mImpact[pop] = impact;
}

int NodeData::getHarbourId() const
{
    return mHarbourId;
}

void NodeData::setHarbourId(int value)
{
    mHarbourId = value;
}

void NodeData::appendAdiancency(int to_id, double weight)
{
    mAdiacency.push_back(to_id);
    mWeights.push_back(weight);
}

void NodeData::removeAdiacencyByIdx(int idx)
{
    mAdiacency.removeAt(idx);
    mWeights.removeAt(idx);
}

void NodeData::removeAdiacencyByTarget(int target)
{
    int idx = mAdiacency.indexOf(target);
    removeAdiacencyByIdx(idx);
}

int NodeData::getAdiacencyCount() const
{
    return mAdiacency.size();
}

int NodeData::getAdiacencyByIdx(int idx) const
{
    return mAdiacency.at(idx);
}

double NodeData::getAdiacencyWeight(int idx) const
{
    return mWeights.at(idx);
}

void NodeData::setAdiacencyWeight(int idx, double w)
{
    mWeights[idx] = w;
}

bool NodeData::isDeleted() const
{
    return mDeleted;
}

void NodeData::setDeleted(bool value)
{
    mDeleted = value;
}



