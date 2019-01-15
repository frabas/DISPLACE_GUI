#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QString>

#include <tuple>

class SimulationRun
{
    QString mOutDir;
    QString mPath;
    QString mName;
    QString mOutName;
    QString mSimName;
    int nSteps = 0;
    int nThreads = 0;
    int mUseStaticPaths=0;

public:
    using DataAsTuple = std::tuple<std::string, std::string,
        std::string, std::string, std::string,
        int, int>;

    SimulationRun();

    explicit SimulationRun(DataAsTuple &&tuple)
        : mOutDir(QString::fromStdString(std::get<0>(tuple))),
          mPath(QString::fromStdString(std::get<1>(tuple))),
          mName(QString::fromStdString(std::get<2>(tuple))),
          mOutName(QString::fromStdString(std::get<3>(tuple))),
          mSimName(QString::fromStdString(std::get<4>(tuple))),
          nSteps(std::get<5>(tuple)),
          nThreads(std::get<6>(tuple))
    {
    }

    explicit SimulationRun(const DataAsTuple &tuple)
        : mOutDir(QString::fromStdString(std::get<0>(tuple))),
          mPath(QString::fromStdString(std::get<1>(tuple))),
          mName(QString::fromStdString(std::get<2>(tuple))),
          mOutName(QString::fromStdString(std::get<3>(tuple))),
          mSimName(QString::fromStdString(std::get<4>(tuple))),
          nSteps(std::get<5>(tuple)),
          nThreads(std::get<6>(tuple))
    {
    }

    QString getOutDir() const;
    void setOutDir(const QString &path);

    QString getPath() const;
    void setPath(const QString &path);

    QString getName() const { return mName; }
    void setName(QString name);

    void setSimulationSteps(int steps);
    int getSimulationSteps() const { return nSteps; }

    void setSimulationOutputName(QString name);
    QString getSimulationOutputName() const { return mOutName; }

    void setUsingStaticPaths(int n);
    int getUsingStaticPaths() const { return mUseStaticPaths; }

    void setSimulationName(QString name);
    QString getSimulationName() const { return mSimName; }

    int getNumThreads() const { return nThreads; }
    void setNumThreads(int n);

    DataAsTuple dataAsTuple() const {
        return std::make_tuple(mOutDir.toStdString(), mPath.toStdString(),
                        mName.toStdString(),
                        mOutName.toStdString(),
                        mSimName.toStdString(),
                        nSteps, nThreads
                        );
    }
};

#endif // SIMULATIONRUN_H
