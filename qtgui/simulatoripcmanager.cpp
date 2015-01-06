#include "simulatoripcmanager.h"

#include <QThread>
#include <QDebug>

SimulatorIpcManager::SimulatorIpcManager(QThread *thread, QObject *parent) :
    QObject(parent),
    mIpcQueue()
{
    connect (thread, SIGNAL(started()), this, SLOT(threadStarted()));
    moveToThread(thread);
}

void SimulatorIpcManager::threadStarted()
{
    char buffer[1024];

    qDebug() << "Thread Started";
    size_t len;
    bool mExit = false;
    while (!mExit) {
        int type = mIpcQueue.pickOrWait(buffer, 1024, &len);
        qDebug() << "got message";
        if (type != ForceExit) {
            switch (type) {
            default:
                qDebug() << "***Got: Message Type " << type << " len " << len;
                break;
            }

        } else {
            mExit = true;
        }
    }

    qDebug() << "Thread ended.";
}
