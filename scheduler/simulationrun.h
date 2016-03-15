#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QString>

class SimulationRun
{
    int nSteps;
    int nThreads;

    QString mName;
    QString mOutName;
    QString mSimName;

public:
    SimulationRun();

    QString getName() const { return mName; }
    void setName(QString name);

    void setSimulationSteps(int steps);
    int getSimulationSteps() const { return nSteps; }

    void setSimulationOutputName(QString name);
    QString getSimulationOutputName() const { return mOutName; }

    void setSimulationName(QString name);
    QString getSimulationName() const { return mSimName; }

    int getNumThreads() { return nThreads; }
    void setNumThreads(int n);
};

#endif // SIMULATIONRUN_H
