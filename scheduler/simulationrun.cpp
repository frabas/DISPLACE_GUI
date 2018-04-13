#include "simulationrun.h"

QString SimulationRun::getOutDir() const
{
    return mOutDir;
}

void SimulationRun::setOutDir(const QString &name)
{
    mOutDir = name;
}

QString SimulationRun::getPath() const
{
    return mPath;
}

void SimulationRun::setPath(const QString &path)
{
    mPath = path;
}

SimulationRun::SimulationRun()
{
    
}

void SimulationRun::setName(QString name)
{
    mName = name;
}

void SimulationRun::setSimulationSteps(int steps)
{
    nSteps = steps;
}


void SimulationRun::setSimulationOutputName(QString name)
{
    mOutName = name;
}

void SimulationRun::setSimulationName(QString name)
{
    mSimName = name;
}

void SimulationRun::setNumThreads(int n)
{
    nThreads = n;
}

