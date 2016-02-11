// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

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
            case MoveShip:
                {
                    displace::ipc::MoveShipMessage *data = reinterpret_cast<displace::ipc::MoveShipMessage *>(buffer);
                    emit shipMoved(data->tstep, data->idx, data->x, data->y, data->course);
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
