#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QProcess>

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

private slots:
    void error(QProcess::ProcessError);
    void finished (int, QProcess::ExitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();

private:
    QProcess *mSimulation;

    bool processCodedLine(QString line);
};

#endif // SIMULATOR_H
