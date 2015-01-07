#ifndef SIMULATORIPCMANAGER_H
#define SIMULATORIPCMANAGER_H

#include <QObject>
#include <ipcqueue.h>

class SimulatorIpcManager : public QObject
{
    Q_OBJECT
public:
    explicit SimulatorIpcManager(QThread *thread, QObject *parent = 0);

    void forceExit();

signals:
    void receivedCodedLine(QString);
    void vesselMoved (int laststep, int idx, float x, float y, float course, float fuel, int state);

public slots:

private slots:
    void threadStarted();

private:
    IpcQueue mIpcQueue;
    QThread *mThread;
};

#endif // SIMULATORIPCMANAGER_H
