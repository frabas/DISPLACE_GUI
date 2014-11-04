#include "populationdata.h"


#include <QDebug>

PopulationData::PopulationData(int _id)
    : id(_id),
      aggregate(), mortality(),
      totAggregate(0.0),
      totMortality(0.0)
{
}

PopulationData::PopulationData(const PopulationData &p)
    : id(p.id),
      aggregate(p.aggregate), mortality(p.mortality),
      totAggregate(p.totAggregate),
      totMortality(p.totMortality)
{

}

const QVector<double> &PopulationData::getAggregate() const
{
    return aggregate;
}

void PopulationData::setAggregate(const QVector<double> &value)
{
    aggregate = value;
}

const QVector<double> &PopulationData::getMortality() const
{
    return mortality;
}

void PopulationData::setMortality(const QVector<double> &value)
{
    mortality = value;
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

double PopulationData::getAggregateTot() const
{
    return totAggregate;
}

void PopulationData::setAggregateTot(double value)
{
    totAggregate = value;
}

