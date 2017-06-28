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
#ifndef NATIONALITYDATA_H
#define NATIONALITYDATA_H

#include <QString>
#include <QVector>

class NationData
{
    QString mName;
public:
    NationData();

    QString getName() const;
    void setName(const QString &value);
};

class NationStats {
public:
    NationStats()
        : mTotCatches(0.0),
          mRevenues(0.0),
          mExRevenues(0.0),
          mTimeAtSea(0), mGav(0), mVpuf(0), mSweptArea(0), mRevenuePerSweptArea(0),
          szGroups()
    {
    }

    double mTotCatches = 0.0;
    double mRevenues = 0.0;
    double mExRevenues = 0.0;
    double mTimeAtSea = 0.0;
    double mGav = 0.0;
    double mVpuf = 0.0;
    double mSweptArea = 0.0;
    double mRevenuePerSweptArea = 0.0;

    QVector<double> szGroups;
};

#endif // NATIONALITYDATA_H
