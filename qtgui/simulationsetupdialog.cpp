#include "simulationsetupdialog.h"
#include "ui_simulationsetupdialog.h"

SimulationSetupDialog::SimulationSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSetupDialog)
{
    ui->setupUi(this);
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
