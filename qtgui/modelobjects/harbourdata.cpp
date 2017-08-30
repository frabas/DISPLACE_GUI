// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "harbourdata.h"

HarbourData::HarbourData(std::shared_ptr<Harbour> harbour)
    : mHarbour(harbour)
{
}


HarbourStats::HarbourStats()
    : mCumCatches(0.0), mCumDiscards(0.0),
      mCumProfit(0.0),
      mGav(0), mVpuf(0), mSweptArea(0), mRevenuePerSweptArea(0), cumVpuf(0), cumRevenuePerSweptArea(0),
      GVA(0),
      cumGVAPerRevenue(0),
      GVAPerRevenue(0),
      LabourSurplus(0),
      GrossProfit(0),
      NetProfit(0),
      cumNetProfitMargin(0),
      NetProfitMargin(0),
      cumRoFTA(0),
      RoFTA(0),
      cumGVAPerFTE(0),
      GVAPerFTE(0),
      cumBER(0),
      BER(0),
      cumCRBER(0),
      CRBER(0),
      NetPresentValue (0),
      numTrips(0),

      szCatches()
{
}
