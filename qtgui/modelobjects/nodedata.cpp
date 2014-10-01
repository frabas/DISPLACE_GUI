#include "nodedata.h"

NodeData::NodeData(Node *nd, DisplaceModel *model)
    : mNode (nd),
      mModel(model),
      mPop(nd ? new double[nd->get_nbpops()] : 0)
{
}

NodeData::~NodeData()
{
    delete []mPop;
}

void NodeData::setPop(int pop, double v)
{
    if (mPop && pop < mNode->get_nbpops() && pop > 0)
        mPop[pop] = v;
}

double NodeData::getPop(int pop) const
{
    if (mPop && pop < mNode->get_nbpops() && pop > 0)
        return mPop[pop];

    return -1;
}

