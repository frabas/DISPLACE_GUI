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

    size_t len;
    bool mExit = false;
    while (!mExit) {
        int type = mIpcQueue.pickOrWait(buffer, 1024, &len);
        if (type != IpcQueue::ForceExit) {
            switch (type) {
            default:
                qDebug() << "***Got: Message Type " << type << " len " << len;
                break;
            }

        } else {
            mExit = true;
        }
    }
}
