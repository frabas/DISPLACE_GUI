#include "nodedata.h"

#include <displacemodel.h>

NodeData::NodeData(Node *nd, DisplaceModel *model)
    : mNode (nd),
      mModel(model),
      mPop(nd ? new double[nd->get_nbpops()] : 0),
      mPopTot(0)
{
    for (int i = 0; i < nd->get_nbpops(); ++i)
        mPop[i] = 0.0;
}

NodeData::~NodeData()
{
    delete []mPop;
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

