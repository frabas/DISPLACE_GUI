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

#include <QGridLayout>
#include <QCheckBox>

PathPenaltyDialog::PathPenaltyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPenaltyDialog)
{
    ui->setupUi(this);

    ui->shapefileGroup->setLayout(mShapefileGrid = new QGridLayout);
    ui->metierGroup->setLayout(mMetierGrid = new QGridLayout);
    ui->enableMetiers->setEnabled(false);

    ui->ok->setEnabled(false);
}

PathPenaltyDialog::~PathPenaltyDialog()
{
    delete ui;
}

void PathPenaltyDialog::showShapefileOptions(bool show)
{
    ui->shapefileGroup->setVisible(show);
}

void PathPenaltyDialog::setShapefileList(QStringList files)
{
    for (auto file : files) {
        auto cb = new QCheckBox(file);
        mShapefileGrid->addWidget(cb);
        mShapefileCheckboxes.push_back(cb);

        connect (cb, SIGNAL(toggled(bool)), this, SLOT(cbToggled(bool)));
    }
}

void PathPenaltyDialog::setMetierNumber(int num)
{
    for (int i = 0; i < num; ++i) {
        auto cb = new QCheckBox(QString(tr("Metier %1")).arg(i+1));
        mMetierGrid->addWidget(cb);
        mMetierCheckboxes.push_back(cb);
    }
}

QStringList PathPenaltyDialog::selectedShapefile() const
{
    QStringList l;
    for (QCheckBox *cb : mShapefileCheckboxes) {
        if (cb->isChecked())
            l << cb->text();
    }

    return l;
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
    if (ui->shapefileGroup->isVisible() && clickCount == 0)
        return;

    accept();
}

void PathPenaltyDialog::cbToggled(bool v)
{
    if (v)
        ++clickCount;
    else
        --clickCount;

    ui->ok->setEnabled(clickCount > 0);
}
