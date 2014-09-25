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

signals:
    void log(QString);
    void processStateChanged(QProcess::ProcessState);
    void simulationStepChanged(int);
    void vesselMoved (int idx, float x, float y, float course, float fuel, int state);

private slots:
    void error(QProcess::ProcessError);
    void finished (int, QProcess::ExitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();

private:
    QProcess *mSimulation;

    bool processCodedLine(QString line);
    void parseUpdateVessel(QString line);
};

#endif // SIMULATOR_H
