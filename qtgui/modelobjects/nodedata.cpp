// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <mapsdataprovider.h>
#include <displacemodel.h>

NodeData::NodeData(std::shared_ptr<Node> nd, DisplaceModel *model)
    : mNode (nd),
      mModel(model),
      mDeleted(false),
      mHarbourId(-1)
{
    if (nd) {
        int N3 = 1; // only one farm per node?
        mFishfarmMeanweight = new double[N3];
        mFishfarmFishHarvestedKg = new double[N3];
        mFishfarmEggsHarvestedKg = new double[N3];
        mFishfarmAnnualProfit = new double[N3];
        mFishfarmNetDischargeN = new double[N3];
        mFishfarmNetDischargeP = new double[N3];
        mFishfarmCumulNetDischargeN = new double[N3];
        mFishfarmCumulNetDischargeP = new double[N3];

        for (int j = 0; j < N3; ++j) {
            mFishfarmMeanweight[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmFishHarvestedKg[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmEggsHarvestedKg[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmAnnualProfit[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmNetDischargeN[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmNetDischargeP[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmCumulNetDischargeN[j] = 0.0;
        }
        for (int j = 0; j < N3; ++j) {
            mFishfarmCumulNetDischargeP[j] = 0.0;
        }


    }
}

NodeData::~NodeData() = default;

std::shared_ptr<types::NodesPopData> NodeData::getNodesData() const
{
    auto tstep = mModel->getCurrentStep();
    auto &dp = mModel->getMapDataProvider();
    return dp.getNodesPopData(get_idx_node(), types::tstep_t(tstep));
}

std::shared_ptr<types::NodesBenthosData> NodeData::getBenthosData() const
{
    auto tstep = mModel->getCurrentStep();
    auto &dp = mModel->getMapDataProvider();
    return dp.getNodesBenthosData(get_idx_node(), types::tstep_t(tstep));
}

int NodeData::getPopCount() const
{
    return mNode->get_nbpops();
}

int NodeData::getMetCount() const
{
    return mNode->get_nbmets();
}

int NodeData::getBenthosPopCount() const
{
    return mNode->get_nbbenthospops();
}


void NodeData::setPop(int pop, double v)
{
}

void NodeData::setPopTot(double tot)
{
}

void NodeData::setPop(QList<double> v, double tot)
{
}

boost::optional<double> NodeData::getPop(int pop) const
{
    auto v = getNodesData();
    if (v && pop < v->totN.size() && pop >= 0)
        return v->totN[pop];

    return boost::none;
}

boost::optional<double> NodeData::getPopTot() const
{
    auto v = getNodesData();
    if (v)
        return std::accumulate(v->totN.begin(), v->totN.end(), 0.0);

    return boost::none;
}

double NodeData::getPopWTot() const
{
    auto v = getNodesData();
    if (v)
        return std::accumulate(v->totW.begin(), v->totW.end(), 0.0);

    return -1;
}

boost::optional<double> NodeData::getImpact(int pop) const
{
    auto v = getNodesData();
    if (v && pop < v->impact.size() && pop >= 0)
        return v->impact[pop];

    return boost::none;
}

boost::optional<double> NodeData::getCumcatchesPerPop(int pop)
{
    auto v = getNodesData();
    if (v && pop < v->cumC.size() && pop >= 0)
        return v->cumC[pop];

    return boost::none;
}


void NodeData::setPopW(int pop, double val)
{
}

void NodeData::setPopWTot(double tot)
{
}

void NodeData::setPopW(QList<double> v, double tot)
{
}

boost::optional<double> NodeData::getPopW(int pop) const
{
    auto v = getNodesData();
    if (v && pop < v->totW.size() && pop >= 0)
        return v->totW[pop];

    return boost::none;
}

void NodeData::setImpact(int pop, double impact)
{
}


void NodeData::setCumcatchesPerPop(int pop, double cumcatchesperpop)
{
}

void NodeData::setCumdiscardsPerPop(int pop, double cumdiscardsperpop)
{
}

void NodeData::setBenthosBiomass(int func, double benthosbiomass)
{
}

void NodeData::setBenthosNumber(int func, double benthosnumber)
{
}

void NodeData::setBenthosMeanweight(int func, double benthosmeanweight)
{
}

void NodeData::setBenthosBiomassOverK(int func, double benthosbiomassoverK)
{
}

void NodeData::setBenthosNumberOverK(int func, double benthosnumberoverK)
{
}

double NodeData::getBenthosBiomass(int func) const
{
    auto v = getBenthosData();
    if (v && func < v->mBenthosBiomass.size() && func >= 0)
        return v->mBenthosBiomass[func];

    return 0;
}

double NodeData::getBenthosNumber(int func) const
{
    auto v = getBenthosData();
    if (v && func < v->mBenthosNumber.size() && func >= 0)
        return v->mBenthosNumber[func];

    return 0;
}

double NodeData::getBenthosMeanweight(int func) const
{
    auto v = getBenthosData();
    if (v && func < v->mBenthosMeanweight.size() && func >= 0)
        return v->mBenthosMeanweight[func];

    return 0;
}

double NodeData::getBenthosBiomassOverK(int func) const
{
    auto v = getBenthosData();
    if (v && func < v->mBenthosBiomassOverK.size() && func >= 0)
        return v->mBenthosBiomassOverK[func];

    return 0;
}

double NodeData::getBenthosNumberOverK(int func) const
{
    auto v = getBenthosData();
    if (v && func < v->mBenthosNumberOverK.size() && func >= 0)
        return v->mBenthosNumberOverK[func];

    return 0;
}

void NodeData::setFishfarmFishMeanWeight(int farm, double fishfarmfishmeanweight)
{
    mFishfarmMeanweight[farm] = fishfarmfishmeanweight;
}

void NodeData::setFishfarmFishHarvestedKg(int farm, double fishfarmfishharvestedkg)
{
    mFishfarmFishHarvestedKg[farm] = fishfarmfishharvestedkg;
}

void NodeData::setFishfarmEggsHarvestedKg(int farm, double fishfarmeggsharvestedkg)
{
    mFishfarmEggsHarvestedKg[farm] = fishfarmeggsharvestedkg;
}

void NodeData::setFishfarmAnnualProfit(int farm, double fishfarmannualprofit)
{
    mFishfarmAnnualProfit[farm] = fishfarmannualprofit;
}

void NodeData::setFishfarmNetDischargeN(int farm, double fishfarmnetdischargeN)
{
    mFishfarmNetDischargeN[farm] = fishfarmnetdischargeN;
}

void NodeData::setFishfarmNetDischargeP(int farm, double fishfarmnetdischargeP)
{
    mFishfarmNetDischargeP[farm] = fishfarmnetdischargeP;
}

void NodeData::setFishfarmCumulNetDischargeN(int farm, double fishfarmcumulnetdischargeN)
{
    mFishfarmCumulNetDischargeN[farm] = fishfarmcumulnetdischargeN;
}

void NodeData::setFishfarmCumulNetDischargeP(int farm, double fishfarmcumulnetdischargeP)
{
    mFishfarmCumulNetDischargeP[farm] = fishfarmcumulnetdischargeP;
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



