#ifndef SIMULATORIPCMANAGER_H
#define SIMULATORIPCMANAGER_H

#include <QObject>
#include <ipcqueue.h>

class SimulatorIpcManager : public QObject
{
    Q_OBJECT
public:
    explicit SimulatorIpcManager(QThread *thread, QObject *parent = 0);

signals:

public slots:

private slots:
    void threadStarted();

private:
    IpcQueue mIpcQueue;
};

#endif // SIMULATORIPCMANAGER_H
