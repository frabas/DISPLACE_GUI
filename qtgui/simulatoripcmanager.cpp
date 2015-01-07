#include "simulatoripcmanager.h"

#include <QThread>
#include <QDebug>

SimulatorIpcManager::SimulatorIpcManager(QThread *thread, QObject *parent) :
    QObject(parent),
    mIpcQueue(),
    mThread(thread)
{
    connect (thread, SIGNAL(started()), this, SLOT(threadStarted()));
    moveToThread(thread);
}

void SimulatorIpcManager::forceExit()
{
    mIpcQueue.push(ForceExit, 0, 0);
}

void SimulatorIpcManager::threadStarted()
{
    char buffer[1024];

    qDebug() << "Thread Started";
    size_t len;
    bool mExit = false;
    while (!mExit) {
        int type = mIpcQueue.pickOrWait(buffer, 1024, &len);
        if (type != ForceExit) {
            switch (type) {
            case GenericConsole:
                {
                    QString msg = QString::fromUtf8(buffer, len);
                    qDebug() << "String message:" << msg;
                    emit receivedCodedLine(msg);
                }
                break;
            default:
                qDebug() << "***Got: Message Type " << type << " len " << len;
                break;
            }

        } else {
            mExit = true;
        }
    }

    qDebug() << "Thread ended.";

    mThread->terminate();
}
