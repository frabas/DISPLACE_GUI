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
#ifndef SIMULATIONSETUPDIALOG_H
#define SIMULATIONSETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SimulationSetupDialog;
}

class SimulationSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationSetupDialog(QWidget *parent = 0);
    ~SimulationSetupDialog();

    void setOutDir(QString outdir);
    QString getOutDir() const;

    void setSimulationSteps(int steps);
    void setUseStaticPaths(int yesno);
    void setPreexistingPathsShop(int yesno);
    int getSimulationSteps() const;
    int getUseStaticPaths() const;
    int getPreexistingPathsShop() const;

    void setSimulationOutputName(QString name);
    QString getSimulationOutputName() const;

    void setSimulationName(QString name);
    QString getSimulationName() const;

    void setMoveVesselsOption(bool x);
    bool getMoveVesselsOption() const;

    int getNumThreads();
    void setNumThreads(int n);

    int getVerbosityLevel();
    void setVerbosityLevel(int value);

    int getVesselMoveDisplayUpdateRate();
    void setVesselMoveDisplayUpdateRate(int value);



private slots:
    void on_verbosity_valueChanged(int arg1);


    void on_threads_valueChanged(int arg1);

private:
    Ui::SimulationSetupDialog *ui;

    void updateMessages();
};

#endif // SIMULATIONSETUPDIALOG_H
