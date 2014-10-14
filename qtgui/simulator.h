#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QProcess>

#include <Vessel.h>
#include <memory>

/** \brief An ongoing simulation process
 *
 */
class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator();

    bool start(QString name, QString folder);
    bool isRunning();
    bool forceStop();
    QProcess::ProcessState processState() const;

    int getSimSteps() const;
    void setSimSteps(int value);

    bool getMoveVesselOption() const;
    void setMoveVesselOption(bool value);

    QString getOutputName() const;
    void setOutputName(const QString &value);

    QString getSimulationName() const;
    void setSimulationName(const QString &value);

signals:
    void log(QString);
    void processStateChanged(QProcess::ProcessState oldstate, QProcess::ProcessState newstate);
    void simulationStepChanged(int);
    void vesselMoved (int laststep, int idx, float x, float y, float course, float fuel, int state);
    void outputFileUpdated(QString, int);
    void nodesStatsUpdate(QString);

private slots:
    void error(QProcess::ProcessError);
    void finished (int, QProcess::ExitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void subprocessStateChanged(QProcess::ProcessState);

private:
    QProcess *mSimulation;
    int mSimSteps;
    int mLastStep;
    QString mOutputName;
    QString mSimuName;
    bool mMoveVesselOption;
    QProcess::ProcessState mProcessState;

    bool processCodedLine(QString line);
    void parseUpdateVessel(QStringList fields);
};

#endif // SIMULATOR_H
