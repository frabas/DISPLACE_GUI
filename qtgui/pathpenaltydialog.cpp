// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

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

namespace {
static const QString Months[] = {
    QT_TR_NOOP("January"), QT_TR_NOOP("February"),
    QT_TR_NOOP("March"), QT_TR_NOOP("April"),
    QT_TR_NOOP("May"), QT_TR_NOOP("June"),
    QT_TR_NOOP("July"), QT_TR_NOOP("August"),
    QT_TR_NOOP("September"), QT_TR_NOOP("October"),
    QT_TR_NOOP("November"), QT_TR_NOOP("December"),
};
}

PathPenaltyDialog::PathPenaltyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPenaltyDialog)
{
    ui->setupUi(this);

    ui->shapefileGroup->setLayout(mShapefileGrid = new QGridLayout);
    ui->metierGroup->setLayout(mMetierGrid = new QGridLayout);

    auto monthsLayout = new QGridLayout;
    for (int i = 0; i < 12; ++i) {
        auto mcb = new QCheckBox(tr(Months[i].toStdString().c_str()));
        monthsLayout->addWidget(mcb, i / 4, i % 4);
        mMonthsCheckboxes.push_back(mcb);
    }
    ui->tabPageMonths->setLayout(monthsLayout);
    ui->tabWidgetPeriods->setCurrentIndex(0);

    ui->ok->setEnabled(false);

    connect (ui->enableMetiers, SIGNAL(toggled(bool)), this, SLOT(metierEnableToggled(bool)));

    ui->enableMetiers->setChecked(true);
    ui->enableMetiers->setChecked(false);   // to force metierEnableToggled call

    ui->enableSizes->setChecked(true);
    ui->enableSizes->setChecked(false);

    ui->enableNations->setChecked(true);

}

PathPenaltyDialog::~PathPenaltyDialog()
{
    delete ui;
}

void PathPenaltyDialog::showShapefileOptions(bool show)
{
    ui->shapefileGroup->setVisible(show);
    ui->ok->setEnabled(!show);
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
    int cols = 8;

    for (int i = 0; i < num; ++i) {
        auto cb = new QCheckBox(QString(tr("Metier %1")).arg(i));
        mMetierGrid->addWidget(cb, i / cols, i % cols);
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


//bool PathPenaltyDialog::isPenaltyQ1()
//{
//    return ui->optPenaltyQ1->isChecked();
//}

//bool PathPenaltyDialog::isPenaltyQ2()
//{
//    return ui->optPenaltyQ2->isChecked();
//}

//bool PathPenaltyDialog::isPenaltyQ3()
//{
//    return ui->optPenaltyQ3->isChecked();
//}

//bool PathPenaltyDialog::isPenaltyQ4()
//{
//    return ui->optPenaltyQ4->isChecked();
//}

double PathPenaltyDialog::weight() const
{
    return ui->weight->value();
}

double PathPenaltyDialog::nbOfDaysClosedPerMonth() const
{
    return ui->nbOfDaysClosedPerMonth->value();
}

std::vector<bool> PathPenaltyDialog::getCheckedMonths() const
{
    std::vector<bool> checkedMonths;
    for (auto mcb : mMonthsCheckboxes)
        checkedMonths.push_back(mcb->isChecked());
    return checkedMonths;
}

std::vector<int> PathPenaltyDialog::getBannedMetiers() const
{
    if (!ui->enableMetiers->isChecked()) {
        return std::vector<int>();
    }

    std::vector<int> r;

    for (int i = 0; (size_t)i < mMetierCheckboxes.size(); ++i) {
        if (mMetierCheckboxes[i]->isChecked())
            r.push_back(i);
    }

    return r;
}

std::vector<int> PathPenaltyDialog::getVesSizeSelection() const
{
    std::vector<int> selection;
    if (ui->cbSize0->isChecked()) selection.push_back(0);
    if (ui->cbSize1->isChecked()) selection.push_back(1);
    if (ui->cbSize2->isChecked()) selection.push_back(2);
    if (ui->cbSize3->isChecked()) selection.push_back(3);
    if (ui->cbSize4->isChecked()) selection.push_back(4);
    return selection;
}

std::vector<int> PathPenaltyDialog::getNationSelection() const
{
    std::vector<int> selection;
    if (ui->cbNation0->isChecked()) selection.push_back(0);
    if (ui->cbNation1->isChecked()) selection.push_back(1);
    if (ui->cbNation2->isChecked()) selection.push_back(2);
    if (ui->cbNation3->isChecked()) selection.push_back(3);
    if (ui->cbNation4->isChecked()) selection.push_back(4);
    if (ui->cbNation5->isChecked()) selection.push_back(5);
    if (ui->cbNation6->isChecked()) selection.push_back(6);
    if (ui->cbNation7->isChecked()) selection.push_back(7);
    if (ui->cbNation8->isChecked()) selection.push_back(8);
    if (ui->cbNation9->isChecked()) selection.push_back(9);
    if (ui->cbNation10->isChecked()) selection.push_back(10);
    if (ui->cbNation11->isChecked()) selection.push_back(11);
    if (ui->cbNation12->isChecked()) selection.push_back(12);
    if (ui->cbNation13->isChecked()) selection.push_back(13);
    if (ui->cbNation14->isChecked()) selection.push_back(14);
    if (ui->cbNation15->isChecked()) selection.push_back(15);
    if (ui->cbNation16->isChecked()) selection.push_back(16);
    if (ui->cbNation17->isChecked()) selection.push_back(17);
    if (ui->cbNation18->isChecked()) selection.push_back(18);
    if (ui->cbNation19->isChecked()) selection.push_back(19);
    if (ui->cbNation20->isChecked()) selection.push_back(20);
    if (ui->cbNation21->isChecked()) selection.push_back(21);
    if (ui->cbNation22->isChecked()) selection.push_back(22);
    if (ui->cbNation23->isChecked()) selection.push_back(23);
    if (ui->cbNation24->isChecked()) selection.push_back(24);
    return selection;
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

void PathPenaltyDialog::metierEnableToggled(bool status)
{
    ui->metierGroup->setEnabled(status);
}

void PathPenaltyDialog::on_enableSizes_toggled(bool enabled)
{
    ui->sizeGroup->setEnabled(enabled);
}

void PathPenaltyDialog::on_enableNations_toggled(bool enabled)
{
    ui->nationGroup->setEnabled(enabled);
}
