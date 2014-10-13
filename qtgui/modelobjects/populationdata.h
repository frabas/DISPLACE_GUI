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

    const QVector<double> &getAggregate() const;
    void setAggregate(const QVector<double> &value);
    const QVector<double> &getMortality() const;
    void setMortality(const QVector<double> &value);
};

#endif // POPULATIONDATA_H
