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

signals:
    void log(QString);
    void processStateChanged(QProcess::ProcessState);
    void simulationStepChanged(int);
    void vesselMoved (int laststep, int idx, float x, float y, float course, float fuel, int state);
    void outputFileUpdated(QString);
    void outputFileUpdated(QString, int);
    void nodesStatsUpdate(QString);

private slots:
    void error(QProcess::ProcessError);
    void finished (int, QProcess::ExitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();

private:
    QProcess *mSimulation;
    int mSimSteps;
    int mLastStep;

    bool processCodedLine(QString line);
    void parseUpdateVessel(QStringList fields);
};

#endif // SIMULATOR_H
