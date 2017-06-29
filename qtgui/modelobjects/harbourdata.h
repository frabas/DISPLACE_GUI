/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef HARBOURDATA_H
#define HARBOURDATA_H

#include <Harbour.h>
#include <QVector>

#include <memory>

class HarbourData
{
public:
    std::shared_ptr<Harbour> mHarbour;

    explicit HarbourData(std::shared_ptr<Harbour> harbour);

    types::NodeId get_idx_node() const { return mHarbour->get_idx_node(); }
    const vector<types::NodeId> &get_usual_fgrounds() { return mHarbour->get_usual_fgrounds(); }
    void set_usual_fgrounds(const vector<types::NodeId> &usual_fgrounds) { mHarbour->set_usual_fgrounds(usual_fgrounds); }
    const vector<double> &get_freq_usual_fgrounds() { return mHarbour->get_freq_usual_fgrounds(); }
    void set_freq_usual_fgrounds(vector<double> freq_usual_fgrounds) { mHarbour->set_freq_usual_fgrounds(freq_usual_fgrounds); }

};



class HarbourStats
{
public:
    HarbourStats();

    double mCumCatches;
    double mCumDiscards;
    double mCumProfit;
    double mGav;
    double mVpuf;
    double mSweptArea;
    double mRevenuePerSweptArea;

    QVector<double> szCatches;
};

#endif // HARBOURDATA_H
