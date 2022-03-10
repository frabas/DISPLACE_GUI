// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "simulator.h"

#include <displacemodel.h>
#include <modelobjects/vesseldata.h>
#include <outputfileparser.h>

#include <QApplication>
#include <QSettings>
#include <QDebug>

#ifndef qInfo
#define qInfo qDebug
#endif

//#define RUN_VALGRIND_ON_CHILD
//#define VALGRIND_TOOL "helgrind"


QString Simulator::SET_NUMTHREADS ("simul_numthreads");
QString Simulator::SET_VERBOSITY ("simulator_verbosity");

Simulator::Simulator()
    : mSimulation(0),
      mIpcThread(0),
      mIpcQueue(0),
      mModel(),
      mSimSteps(8761),
      useStaticPaths(1),
      mLastStep(-1),
      mOutDir("C:"),
      mOutputName("baseline"),
      mSimuName("simu1"),
      mMoveVesselOption(true),
      mVesselMoveDisplayUpdateRate(1),
      mProcessState(QProcess::NotRunning),
      mCapture(false),
      mVerbosity(0)
{
}

Simulator::~Simulator()
{
    mIpcQueue->forceExit();
    while (mIpcThread->isRunning()) {
        qDebug() << "ipc still running";
        mIpcThread->terminate();
        mIpcThread->wait(3000);
    }
}

void Simulator::linkModel(std::shared_ptr<DisplaceModel> model)
{
    mModel= model;
}

// -f "myfish" --f2 "baseline" -s "simu2" -i 8761 -p 1 -e 1 --huge 1 -v 0 --without-gnuplot -V 0 --num_threads 8 --outdir "D:"
bool Simulator::start(QString outdir, QString name, QString folder, QString simul_name)
{
    if (mSimulation != 0) {
        delete mSimulation;
        mSimulation = 0;
    }
    if (mIpcThread != 0) {
        delete mIpcThread;
    }
    if (mIpcQueue != 0) {
        delete mIpcQueue;
    }

    mIpcThread = new QThread(this);
    mIpcThread->setObjectName("IpcThread");

    try {
        mIpcQueue = new SimulatorIpcManager(mIpcThread);
    } catch (boost::interprocess::bad_alloc &xc) {
        qFatal("Can't allocate memory %s", xc.what());
        return false;
    }

    connect (mIpcQueue, SIGNAL(receivedCodedLine(QString)), this, SLOT(processCodedLine(QString)));
    connect (mIpcQueue, SIGNAL(vesselMoved(int,int,float,float,float,float,int)), SIGNAL(vesselMoved(int,int,float,float,float,float,int)));
    connect (mIpcQueue, SIGNAL(shipMoved(int,int,float,float,float)), SIGNAL(shipMoved(int,int,float,float,float)));
    connect (mIpcQueue, SIGNAL(vesselLogbookReceived(VesselStats)), this, SLOT(vesselLogbookReceived(VesselStats)));

    mIpcThread->start();

    if (!outdir.isEmpty())
        mOutDir = outdir;
    mSimuName = simul_name;
    mSimulation = new QProcess();

    QStringList arguments;
    QSettings set;

    QString executable (QApplication::applicationDirPath() + "/displace");

#if defined(RUN_VALGRIND_ON_CHILD)
    executable = "valgrind";
    arguments.push_back("--tool=" VALGRIND_TOOL);
    arguments.push_back(QApplication::applicationDirPath() + "/displace");
#endif

    if (!outdir.isEmpty()) {
        arguments.push_back("--outdir");
        arguments.push_back(outdir);
    }
    arguments.push_back("-f");
    arguments.push_back(name);

    arguments.push_back("--f2");
    arguments.push_back(mOutputName);
    arguments.push_back("-s");
    arguments.push_back(simul_name);
    arguments.push_back("-i");
    arguments.push_back(QString::number(mSimSteps));
    arguments.push_back("-p");
    arguments.push_back(QString::number(useStaticPaths)); // Changeme
    arguments.push_back("-e");
    arguments.push_back("1"); // Changeme       // export vmslike
    arguments.push_back("--huge");
    arguments.push_back("1"); // Changeme       // export huge files
    arguments.push_back("-v");
    arguments.push_back("0"); // Changeme
    arguments.push_back("--use-gui");
    arguments.push_back("-V");
    arguments.push_back(QString::number(mVerbosity));

    if (!mMoveVesselOption)
        arguments.push_back("--no-gui-move-vessels");

    arguments.push_back("--rate");
    arguments.push_back(QString::number(mVesselMoveDisplayUpdateRate));

    arguments.push_back("--num_threads");
    arguments.push_back(QString::number(set.value(SET_NUMTHREADS, 4).toInt()));

#ifdef DEBUG
    arguments.push_back("--debug");
#endif

    connect(mSimulation, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(mSimulation, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(mSimulation, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(mSimulation, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(mSimulation, SIGNAL(started()), this, SLOT(started()));
    connect(mSimulation, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(mSimulation, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(subprocessStateChanged(QProcess::ProcessState)));

    qInfo() << "Running: " << executable << "from" << folder << " with arguments: " << arguments.join(" ");
    mSimulation->setWorkingDirectory(folder);

    mSimulation->start(executable, arguments);

    return true;
}

bool Simulator::isRunning()
{
    return mSimulation != 0 && mSimulation->state() == QProcess::Running;
}

bool Simulator::forceStop()
{
    if (mSimulation == 0)
        return false;

    mSimulation->kill();
    return true;
}

QProcess::ProcessState Simulator::processState() const
{
    return mSimulation ? mSimulation->state() : QProcess::NotRunning;
}

void Simulator::error(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    qWarning() << "Process Error: " << error << mSimulation->errorString();
    emit log(QString("Process error: %1").arg(mSimulation->errorString()));
}

void Simulator::finished(int code, QProcess::ExitStatus status)
{
    mIpcQueue->forceExit();
    emit log(QString("Process exited %1 with exit status %2")
             .arg(status == QProcess::NormalExit ? "normally" : "by crash")
             .arg(code));
    emit simulationEnded(code);
}

void Simulator::readyReadStandardError()
{
    QByteArray data = mSimulation->readAllStandardError();

    QString msg(data.constData());
    emit log("ERR:" + msg);
}

void Simulator::readyReadStandardOutput()
{
    char buf[65536];

    while (mSimulation->readLine(buf, sizeof(buf)) > 0) {
        QString line (buf);
        if (!processCodedLine(line)) {
            emit log(line);
            if (mCapture) {
                emit debugCapture(line);
            }
        }
    }
}

void Simulator::started()
{
    emit log(QString("Process started"));
}

void Simulator::subprocessStateChanged(QProcess::ProcessState state)
{
    QProcess::ProcessState oldstate = mProcessState;
    mProcessState = state;

    emit processStateChanged(oldstate, mProcessState);
}

bool Simulator::wasSimulationStarted() const
{
    return mSimulation != 0;
}

void Simulator::setOutDir(const QString &value)
{
    mOutDir = value;
}

void Simulator::setOutputName(const QString &value)
{
    mOutputName = value;
}

int Simulator::getVesselMoveDisplayUpdateRate() const
{
    return mVesselMoveDisplayUpdateRate;
}

void Simulator::setVesselMoveDisplayUpdateRate(int value)
{
    mVesselMoveDisplayUpdateRate = value;
}

bool Simulator::getMoveVesselOption() const
{
    return mMoveVesselOption;
}

void Simulator::setMoveVesselOption(bool value)
{
    mMoveVesselOption = value;
}

void Simulator::setSimSteps(int value)
{
    mSimSteps = value;
}

void Simulator::setUseStaticPaths(int value)
{
    useStaticPaths = value;
}


bool Simulator::processCodedLine(QString line)
{
    if (!line.startsWith("="))
        return false;

    QStringList args = line.trimmed().mid(2).split(" ", QString::SkipEmptyParts);

    switch(line.at(1).toLatin1()) {
    case 'S':
        mLastStep = args[0].toInt();
        emit simulationStepChanged(mLastStep);
        break;

    case 'V':
        parseUpdateVessel(args);
        break;

    case 'v':
        parseUpdateVesselStats(args);
        break;

    case 'X':
        parseUpdateShip(args);
        break;

    case 'U':
        emit outputFileUpdated(args[0], args[1].toInt());
        break;

    case 'N':
        emit nodesStatsUpdate(line.mid(2));
        break;

    case 'D':
        parseDebug(args);
        break;

    case 'O':
        parseOutputSpecs(line.mid(2));
        break;

    default:
        return false;
    }

    return true;
}

void Simulator::vesselLogbookReceived(VesselStats v)
{
    Q_UNUSED(v);
    //TODO remove this function
}

void Simulator::parseDebug(QStringList fields)
{
    switch (fields[0].at(0).toLatin1()) {
    case 'm':
        if (fields.size() > 1) {
            long rss = fields[0].mid(1).toDouble();
            long peak = fields[1].toDouble();
            emit debugMemoryStats(rss,peak);
        }
        break;
    case 'c':
        if (fields[0].at(1) == '+') {
            mCapture = true;
        } else {
            mCapture = false;
        }
        break;
    }
}

void Simulator::parseOutputSpecs(QString details)
{
    auto d = details.left(4);
    auto str = details.mid(4);

    if (d == "sql:") {
        emit sqliteStorageOpened(str);
    } else {
        // just print on the console
        qDebug() << "Output Specs: " << d << " => " << str;
    }
}

void Simulator::parseUpdateVessel(QStringList fields)
{
    int id = fields[1].toInt();
    float x = fields[3].toFloat();
    float y = fields[4].toFloat();
    float course = fields[5].toFloat();
    float fuel = fields[6].toFloat();
    int state = fields[7].toInt();

    emit vesselMoved(mLastStep, id, x, y, course, fuel, state);
}

void Simulator::parseUpdateVesselStats(QStringList fields)
{
    //TODO Remove this function
    Q_UNUSED(fields);
}

void Simulator::parseUpdateShip(QStringList fields)
{
    int id = fields[1].toInt();
    float x = fields[3].toFloat();
    float y = fields[4].toFloat();
    float course = fields[5].toFloat();

    emit shipMoved(mLastStep, id, x, y, course);
}

