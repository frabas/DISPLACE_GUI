// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QProcess>

#include <simulatoripcmanager.h>
#include <Vessel.h>
#include <memory>

class DisplaceModel;
class OutputFileParser;

/** \brief An ongoing simulation process
 *
 */
class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator();
    ~Simulator();

    void linkModel (std::shared_ptr<DisplaceModel> model);
    bool start(QString outdir, QString name, QString folder, QString simul_name);
    bool isRunning();
    bool forceStop();
    QProcess::ProcessState processState() const;

    void setSimSteps(int value);
    void setUseStaticPaths(int value);
    void setPreexistingPathsShop(int value);
    void setVerbosityLevel(int value) { mVerbosity = value; }
   
    bool getMoveVesselOption() const;
    void setMoveVesselOption(bool value);

    int getVesselMoveDisplayUpdateRate() const;
    void setVesselMoveDisplayUpdateRate(int value);

    void setOutDir(const QString &value);
    void setOutputName(const QString &value);

    bool wasSimulationStarted() const;

signals:
    void log(QString);
    void simulationEnded (int status);
    void processStateChanged(QProcess::ProcessState oldstate, QProcess::ProcessState newstate);
    void simulationStepChanged(int);
    void vesselMoved (int laststep, int idx, float x, float y, float course, float fuel, int state);
    void shipMoved (int laststep, int idx, float x, float y, float course);
    void outputFileUpdated(QString, int);
    void nodesStatsUpdate(QString);
    void debugMemoryStats(long rss, long peak);
    void debugCapture(QString);
    void sqliteStorageOpened(QString);

private slots:
    void error(QProcess::ProcessError);
    void finished (int, QProcess::ExitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void subprocessStateChanged(QProcess::ProcessState);
    bool processCodedLine(QString line);
    void vesselLogbookReceived(VesselStats);

private:
    QProcess *mSimulation;
    QThread *mIpcThread;
    SimulatorIpcManager *mIpcQueue;
    std::shared_ptr<DisplaceModel> mModel;
    int mSimSteps;
    int useStaticPaths;
    int mLastStep;
    QString mOutDir;
    QString mOutputName;
    QString mSimuName;
    bool mMoveVesselOption;
    int mVesselMoveDisplayUpdateRate;
    QProcess::ProcessState mProcessState;
    bool mCapture;
    int mVerbosity;
   
    void parseUpdateVessel(QStringList fields);
    void parseUpdateShip(QStringList fields);
    void parseUpdateVesselStats(QStringList fields);
    void parseDebug(QStringList fields);
    void parseOutputSpecs(QString details);

public:
    static QString SET_NUMTHREADS;
    static QString SET_VERBOSITY;
};

#endif // SIMULATOR_H
