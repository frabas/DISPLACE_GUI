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

public slots:

private slots:
    void threadStarted();

private:
    IpcQueue mIpcQueue;
    QThread *mThread;
};

#endif // SIMULATORIPCMANAGER_H
