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

#include "populationdata.h"


#include <QDebug>

PopulationData::PopulationData(int _id)
    : id(_id),
      aggregate(0), mortality(0), SSB(),
      totAggregate(0.0),
      totMortality(0.0),
      totSSB(0.0)
{
}

PopulationData::PopulationData(const PopulationData &p)
    : id(p.id),
      aggregate(p.aggregate), mortality(p.mortality), SSB(p.SSB),
      totAggregate(p.totAggregate),
      totMortality(p.totMortality),
      totSSB(p.totSSB)
{

}

QString PopulationData::getName() const
{
    return mName;
}

void PopulationData::setName(const QString &value)
{
    mName = value;
}

const QVector<double> &PopulationData::getAggregate() const
{
    return aggregate;
}

double PopulationData::getAggregateAt(int i) const
{
    if (aggregate.size() > i)
        return aggregate.at(i);
    return 0;
}

void PopulationData::setAggregate(const QVector<double> &value)
{
    aggregate = value;
    // force aggregate and mortality to have the same size!
    while (mortality.size() < aggregate.size())
        mortality.push_back(0);
}

const QVector<double> &PopulationData::getMortality() const
{
    return mortality;
}

double PopulationData::getMortalityAt(int i) const
{
    if (mortality.size() > i)
        return mortality.at(i);
    return 0;
}

void PopulationData::setMortality(const QVector<double> &value)
{
    mortality = value;
    // force aggregate and mortality to have the same size!
    while (aggregate.size() < mortality.size())
        aggregate.push_back(0);
}

const QVector<double> &PopulationData::getSSB() const
{
    return SSB;
}

void PopulationData::setSSB(const QVector<double> &value)
{
    SSB = value;
    // force aggregate and mortality to have the same size!
    while (aggregate.size() < SSB.size()){
        aggregate.push_back(0);
        mortality.push_back(0);
     }
}


int PopulationData::getId() const
{
    return id;
}

double PopulationData::getMortalityTot() const
{
    return totMortality;
}

void PopulationData::setMortalityTot(double value)
{
    totMortality = value;
}

double PopulationData::getSSBTot() const
{
    return totSSB;
}

void PopulationData::setSSBTot(double value)
{
    totSSB = value;
}


double PopulationData::getAggregateTot() const
{
    return totAggregate;
}

void PopulationData::setAggregateTot(double value)
{
    totAggregate = value;
}

