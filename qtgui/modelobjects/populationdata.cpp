#include "populationdata.h"

PopulationData::PopulationData(int _id)
    : id(_id),
      aggregate(0.0),
      mortality(0.0)
{
}

PopulationData::PopulationData(const PopulationData &p)
    : id(p.id),
      aggregate(p.aggregate),
      mortality(p.mortality)
{

}

int PopulationData::getId() const
{
    return id;
}

double PopulationData::getMortality() const
{
    return mortality;
}

void PopulationData::setMortality(double value)
{
    mortality = value;
}

double PopulationData::getAggregate() const
{
    return aggregate;
}

void PopulationData::setAggregate(double value)
{
    aggregate = value;
}

