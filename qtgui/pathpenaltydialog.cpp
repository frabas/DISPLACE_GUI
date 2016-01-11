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

#include "pathpenaltydialog.h"
#include "ui_pathpenaltydialog.h"

PathPenaltyDialog::PathPenaltyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPenaltyDialog)
{
    ui->setupUi(this);

    on_shapefile_currentIndexChanged(0);
}

PathPenaltyDialog::~PathPenaltyDialog()
{
    delete ui;
}

void PathPenaltyDialog::showShapefileOptions(bool show)
{
    ui->shapefile->setVisible(show);
    ui->label_shapefile->setVisible(show);
}

void PathPenaltyDialog::setShapefileList(QStringList files)
{
    ui->shapefile->addItems(files);
}

QString PathPenaltyDialog::selectedShapefile() const
{
    return ui->shapefile->currentText();
}

bool PathPenaltyDialog::isClosedForFishing()
{
    return ui->optClosedForFishing->isChecked();
}

bool PathPenaltyDialog::isPenaltyQ1()
{
    return ui->optPenaltyQ1->isChecked();
}

bool PathPenaltyDialog::isPenaltyQ2()
{
    return ui->optPenaltyQ2->isChecked();
}

bool PathPenaltyDialog::isPenaltyQ3()
{
    return ui->optPenaltyQ3->isChecked();
}

bool PathPenaltyDialog::isPenaltyQ4()
{
    return ui->optPenaltyQ4->isChecked();
}

double PathPenaltyDialog::weight() const
{
    return ui->weight->value();
}

void PathPenaltyDialog::on_ok_clicked()
{
    if (ui->shapefile->isVisible() && ui->shapefile->currentIndex() == -1)
        return;

    accept();
}

void PathPenaltyDialog::on_shapefile_currentIndexChanged(int)
{
    ui->ok->setEnabled(!ui->shapefile->isVisible() || ui->shapefile->currentIndex() != -1);
}
