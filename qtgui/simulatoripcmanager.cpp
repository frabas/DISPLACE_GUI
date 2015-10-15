#include "simulatoripcmanager.h"

#include <QThread>
#include <QDebug>
#include <modelobjects/vesseldata.h>

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
                    //                    qDebug() << "String message:" << msg;
                    emit receivedCodedLine(msg);
                }
                break;
            case MoveVessel:
                {
                    displace::ipc::MoveVesselMessage *data = reinterpret_cast<displace::ipc::MoveVesselMessage *>(buffer);
                    emit vesselMoved(data->tstep, data->idx, data->x, data->y, data->course, data->cumfuelcons, data->state);
                }
                break;
            case VesselLogbook:
                {
                    displace::ipc::VesselLogbookMessage *data = reinterpret_cast<displace::ipc::VesselLogbookMessage *>(buffer);
                    VesselStats stats;
                    stats.tstep = data->tstep;
                    stats.reasonToGoBack = data->rtbb;
                    stats.lastHarbour = data->node;
                    stats.vesselId = data->idx;
                    stats.timeAtSea = data->timeatsea;
                    stats.cumFuelCons = data->cumfcons;

                    int pop = data->popnum;
                    for (int i = 0; i < pop; ++i) {
                        stats.mCatches.push_back(data->pop[i]);
                    }
                    stats.revenueAV = data->revenue_from_av_prices;
                    stats.revenueExAV = data->revenue_explicit_from_av_prices;
                    stats.fuelCost = data->fuelcost;
                    stats.gav = data->gav2;

                    emit vesselLogbookReceived(stats);
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
