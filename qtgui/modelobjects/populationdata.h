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
#ifndef POPULATIONDATA_H
#define POPULATIONDATA_H

#include <QVector>

class PopulationData
{
    int id;
    QVector<double> aggregate;
    QVector<double> mortality;

    double totAggregate, totMortality;

public:
    PopulationData()
        : id(-1), aggregate(), mortality(), totAggregate(0.0), totMortality(0.0) { }

    explicit PopulationData(int _id);
    PopulationData(const PopulationData&p);

    int getId() const;

    double getAggregateTot() const;
    void setAggregateTot(double value);

    double getMortalityTot() const;
    void setMortalityTot(double value);

    void clear() {
        for (int i = 0; i < aggregate.size(); ++i)
            aggregate[i] = 0;
        for (int i = 0; i < mortality.size(); ++i)
            mortality[i] = 0;
        totAggregate = totMortality = 0;
    }

    const QVector<double> &getAggregate() const;
    void setAggregate(const QVector<double> &value);
    const QVector<double> &getMortality() const;
    void setMortality(const QVector<double> &value);
};

#endif // POPULATIONDATA_H
