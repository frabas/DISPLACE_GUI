#include "simulationsetupdialog.h"
#include "ui_simulationsetupdialog.h"

#include <QThread>

SimulationSetupDialog::SimulationSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSetupDialog)
{
    ui->setupUi(this);

    ui->outName->setReadOnly(true);

    int x = QThread::idealThreadCount() - 4;
    ui->labelThreadHint->setText(QString(tr("The suggested number of thread for this processor is %1")).arg(x >= 1 ? x : 1));
}

SimulationSetupDialog::~SimulationSetupDialog()
{
    delete ui;
}

void SimulationSetupDialog::setSimulationSteps(int steps)
{
    ui->simSteps->setValue(steps);
}

int SimulationSetupDialog::getSimulationSteps() const
{
    return ui->simSteps->value();
}

void SimulationSetupDialog::setSimulationOutputName(QString name)
{
    ui->outName->setText(name);
}

QString SimulationSetupDialog::getSimulationOutputName() const
{
    return ui->outName->text();
}

void SimulationSetupDialog::setSimulationName(QString name)
{
    ui->simName->setText(name);
}

QString SimulationSetupDialog::getSimulationName() const
{
    return ui->simName->text();
}

void SimulationSetupDialog::setMoveVesselsOption(bool x)
{
    ui->optMoveVessels->setChecked(x);
}

bool SimulationSetupDialog::getMoveVesselsOption() const
{
    return ui->optMoveVessels->isChecked();
}

int SimulationSetupDialog::getNumThreads()
{
    return ui->threads->value();
}

void SimulationSetupDialog::setNumThreads(int n)
{
    ui->threads->setValue(n);
}
