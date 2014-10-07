#ifndef POPULATIONDATA_H
#define POPULATIONDATA_H

class PopulationData
{
    int id;
    double aggregate;
    double mortality;

public:
    explicit PopulationData(int _id);

    int getId() const;

    double getAggregate() const;
    void setAggregate(double value);

    double getMortality() const;
    void setMortality(double value);
};

#endif // POPULATIONDATA_H
