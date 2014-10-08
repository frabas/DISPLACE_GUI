#ifndef POPULATIONDATA_H
#define POPULATIONDATA_H

class PopulationData
{
    int id;
    double aggregate;
    double mortality;

public:
    PopulationData()
        : id(-1), aggregate(0.0), mortality(0.0) { }

    explicit PopulationData(int _id);
    PopulationData(const PopulationData&p);

    int getId() const;

    double getAggregate() const;
    void setAggregate(double value);

    double getMortality() const;
    void setMortality(double value);
};

#endif // POPULATIONDATA_H
