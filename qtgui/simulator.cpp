#include "simulator.h"

#include <QApplication>
#include <QDebug>

Simulator::Simulator()
    : mSimulation(0),
      mSimSteps(8761)
{
}

// -f "balticonly" -f2 "baseline" -s "simu2" -i 8761 -p 1 -o 1 -e 0 -v 0 --without-gnuplot
bool Simulator::start(QString name, QString folder)
{
    if (mSimulation != 0)
        return false;

    mSimulation = new QProcess();

    QStringList arguments;

    arguments.push_back("-f");
    arguments.push_back(name);

    arguments.push_back("-f2");
    arguments.push_back("baseline"); // Changeme
    arguments.push_back("-s");
    arguments.push_back("simu2"); // Changeme
    arguments.push_back("-i");
    arguments.push_back(QString("%1").arg(mSimSteps)); // Changeme
    arguments.push_back("-p");
    arguments.push_back("1"); // Changeme
    arguments.push_back("-o");
    arguments.push_back("1"); // Changeme
    arguments.push_back("-e");
    arguments.push_back("1"); // Changeme       // export vmslike
    arguments.push_back("-v");
    arguments.push_back("0"); // Changeme
    arguments.push_back("--without-gnuplot");
    arguments.push_back("--use-gui");

    connect(mSimulation, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(mSimulation, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(mSimulation, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(mSimulation, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(mSimulation, SIGNAL(started()), this, SLOT(started()));
    connect(mSimulation, SIGNAL(stateChanged(QProcess::ProcessState)), SIGNAL(processStateChanged(QProcess::ProcessState)));

    qDebug() << "Running: " << (QApplication::applicationDirPath() + "/displace" ) << "from" << folder << " with arguments: " << arguments;
    mSimulation->setWorkingDirectory(folder);

#ifdef __MAC_10_0
    QProcessEnvironment env = mSimulation->processEnvironment();
    env.insert("DYLD_LIBRARY_PATH", QApplication::applicationDirPath());
    mSimulation->setProcessEnvironment(env);
#endif

    mSimulation->start(QApplication::applicationDirPath() + "/displace", arguments);

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
    emit log(QString("Process error: %1").arg(mSimulation->errorString()));
}

void Simulator::finished(int, QProcess::ExitStatus)
{

}

void Simulator::readyReadStandardError()
{
    QByteArray data = mSimulation->readAllStandardError();

    QString msg(data.constData());
    emit log("ERR:" + msg);
}

void Simulator::readyReadStandardOutput()
{
    char buf[1024];

    while (mSimulation->readLine(buf, sizeof(buf)) > 0) {
        QString line (buf);
        if (!processCodedLine(line))
            emit log(line);
    }
}

void Simulator::started()
{

}
int Simulator::getSimSteps() const
{
    return mSimSteps;
}

void Simulator::setSimSteps(int value)
{
    mSimSteps = value;
}


bool Simulator::processCodedLine(QString line)
{
    if (!line.startsWith("="))
        return false;

    switch(line.at(1).toLatin1()) {
    case 'S':
        emit simulationStepChanged(line.mid(2).toInt());
        break;

    case 'V':
        parseUpdateVessel(line.mid(2));
        break;

    case 'U':
        emit outputFileUpdated(line.mid(2));
        break;

    default:
        return false;
    }

    return true;
}

void Simulator::parseUpdateVessel(QString line)
{
    QStringList fields = line.split(",");
    int id = fields[1].toInt();
    float x = fields[3].toFloat();
    float y = fields[4].toFloat();
    float course = fields[5].toFloat();
    float fuel = fields[6].toFloat();
    int state = fields[7].toInt();

    emit vesselMoved(id, x, y, course, fuel, state);
}
