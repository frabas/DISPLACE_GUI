#include "simulator.h"

#include <QApplication>
#include <QDebug>

Simulator::Simulator()
    : mSimulation(0)
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
    arguments.push_back("8761"); // Changeme
    arguments.push_back("-p");
    arguments.push_back("1"); // Changeme
    arguments.push_back("-o");
    arguments.push_back("1"); // Changeme
    arguments.push_back("-e");
    arguments.push_back("0"); // Changeme
    arguments.push_back("-v");
    arguments.push_back("0"); // Changeme
    arguments.push_back("--without-gnuplot");

    connect(mSimulation, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(mSimulation, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(mSimulation, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(mSimulation, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(mSimulation, SIGNAL(started()), this, SLOT(started()));
    connect(mSimulation, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)));

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
    QByteArray data = mSimulation->readAllStandardOutput();

    QString msg(data.constData());
    emit log(msg);
}

void Simulator::started()
{

}

void Simulator::stateChanged(QProcess::ProcessState)
{

}

