// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "nodedata.h"

#include <displacemodel.h>

NodeData::NodeData(std::shared_ptr<Node> nd, DisplaceModel *model)
    : mNode (nd),
      mModel(model),
      mDeleted(false),
      mHarbourId(-1),
      mPop(0),
      mPopTot(0),
      mPopW(0),
      mPopWTot(0)
{
    if (nd) {
        int N = nd->get_nbpops();
        int N2 = nd->get_nbbenthospops();
        mPop = new double[N] ;
        mPopW = new double[N] ;
        mImpact = new double[N];
        mBenthosBiomass = new double[N2];
        for (int i = 0; i < N; ++i) {
            mPop[i] = 0.0;
            mPopW[i] = 0.0;
            mImpact[i] = 0.0;
        }
        for (int j = 0; j < N2; ++j) {
            mBenthosBiomass[j] = 0.0;
        }

    }
}

NodeData::~NodeData()
{
    delete []mPop;
    delete []mPopW;
    delete []mImpact;
    delete []mBenthosBiomass;
}

int NodeData::getPopCount() const
{
    return mNode->get_nbpops();
}

int NodeData::getBenthosPopCount() const
{
    return mNode->get_nbbenthospops();
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

void NodeData::setBenthosBiomass(int func, double benthosbiomass)
{
    mBenthosBiomass[func] = benthosbiomass;
}

int NodeData::getHarbourId() const
{
    return mHarbourId;
}

void NodeData::setHarbourId(int value)
{
    mHarbourId = value;
}

void NodeData::setAreaType(int value)
{
    areaType = value;
}




/**
 * @brief NodeData::appendAdiancency
 * @param to_id
 * @param weight
 * @return Index of the appended adiacency
 */
int NodeData::appendAdiancency(std::shared_ptr<NodeData> target, double weight)
{
    mAdiacency.push_back(std::shared_ptr<Edge>(new Edge(shared_from_this(), target, weight)));
    return mAdiacency.size() -1;
}

void NodeData::removeAdiacencyByIdx(int idx)
{
    mAdiacency.removeAt(idx);
}

void NodeData::removeAdiacencyByTarget(std::shared_ptr<NodeData> target)
{
    for (int i = 0; i < mAdiacency.size(); ++i) {
        std::shared_ptr<NodeData> tg = mAdiacency.at(i)->target.lock();
        if (tg == target) {
            removeAdiacencyByIdx(i);
            return;
        }
    }
}

void NodeData::removeAllAdiacencies()
{
    mAdiacency.clear();
}

int NodeData::getAdiacencyCount() const
{
    return mAdiacency.size();
}

std::shared_ptr<NodeData::Edge> NodeData::getAdiacencyByIdx(int idx) const
{
    return mAdiacency.at(idx);
}

double NodeData::getAdiacencyWeight(int idx) const
{
    return mAdiacency.at(idx)->weight;
}

void NodeData::setAdiacencyWeight(int idx, double w)
{
    mAdiacency[idx]->weight = w;
}

bool NodeData::isDeleted() const
{
    return mDeleted;
}

void NodeData::setDeleted(bool value)
{
    mDeleted = value;
}



