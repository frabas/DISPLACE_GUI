// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "simulationsetupdialog.h"
#include "ui_simulationsetupdialog.h"

#include <QThread>

SimulationSetupDialog::SimulationSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSetupDialog)
{
    ui->setupUi(this);

    ui->outName->setReadOnly(true);

    updateMessages();
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

void SimulationSetupDialog::setOutDir(QString outdir)
{
    ui->outDir->setText(outdir);
}


QString SimulationSetupDialog::getOutDir() const
{
    return ui->outDir->text();
}

void SimulationSetupDialog::setUseStaticPaths(int yesno)
{
    ui->useStaticPaths->setValue(yesno);
}

void SimulationSetupDialog::setPreexistingPathsShop(int yesno)
{
    ui->preexistingPathsShop->setValue(yesno);
}



int SimulationSetupDialog::getUseStaticPaths() const
{
    return ui->useStaticPaths->value();
}

int SimulationSetupDialog::getPreexistingPathsShop() const
{
    return ui->preexistingPathsShop->value();
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


int SimulationSetupDialog::getVesselMoveDisplayUpdateRate()
{
    return ui->vesselMoveDisplayUpdateRate->value();
}

void SimulationSetupDialog::setVesselMoveDisplayUpdateRate(int value)
{
    ui->vesselMoveDisplayUpdateRate->setValue(value);
}

int SimulationSetupDialog::getVerbosityLevel()
{
    return ui->verbosity->value();
}

void SimulationSetupDialog::setVerbosityLevel(int value)
{
    ui->verbosity->setValue(value);
}

void SimulationSetupDialog::updateMessages()
{
#ifdef __unix
    int x = QThread::idealThreadCount() - 4;
#else
    int x = QThread::idealThreadCount();
#endif

    if (x != ui->threads->value())
        ui->labelThreadHint->setText(QString(tr("The suggested number of thread for this processor is %1")).arg(x >= 1 ? x : 1));
    else
        ui->labelThreadHint->clear();

    if (ui->verbosity->value() > 0 && ui->threads->value() != 1)
        ui->labelVerbosityWarning->setText(QString(tr("Warning: if verbosity messages are enabled and multiple threads are running, messages can be mixed and be unreadable.")));
    else
        ui->labelVerbosityWarning->clear();

}

void SimulationSetupDialog::on_verbosity_valueChanged(int)
{
    updateMessages();
}

void SimulationSetupDialog::on_threads_valueChanged(int)
{
    updateMessages();
}
