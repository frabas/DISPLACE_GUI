#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QString>

#include <tuple>

class SimulationRun
{
    int nSteps = 0;
    int nThreads = 0;

    QString mPath;
    QString mName;
    QString mOutName;
    QString mSimName;

public:
    using DataAsTuple = std::tuple<std::string,
        std::string, std::string, std::string,
        int, int>;

    SimulationRun();

    explicit SimulationRun(DataAsTuple &&tuple)
        : mPath(QString::fromStdString(std::get<0>(tuple))),
          mName(QString::fromStdString(std::get<1>(tuple))),
          mOutName(QString::fromStdString(std::get<2>(tuple))),
          mSimName(QString::fromStdString(std::get<3>(tuple))),
          nSteps(std::get<4>(tuple)),
          nThreads(std::get<5>(tuple))
    {
    }

    explicit SimulationRun(const DataAsTuple &tuple)
        : mPath(QString::fromStdString(std::get<0>(tuple))),
          mName(QString::fromStdString(std::get<1>(tuple))),
          mOutName(QString::fromStdString(std::get<2>(tuple))),
          mSimName(QString::fromStdString(std::get<3>(tuple))),
          nSteps(std::get<4>(tuple)),
          nThreads(std::get<5>(tuple))
    {
    }

    QString getPath() const;
    void setPath(const QString &path);

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

    DataAsTuple dataAsTuple() const {
        return std::make_tuple(mPath.toStdString(),
                        mName.toStdString(),
                        mOutName.toStdString(),
                        mSimName.toStdString(),
                        nSteps, nThreads
                        );
    }
};

#endif // SIMULATIONRUN_H
