// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "creategraphdialog.h"
#include "ui_creategraphdialog.h"

#include <QMessageBox>
#include <QButtonGroup>
#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>

CreateGraphDialog::CreateGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGraphDialog)
{
    ui->setupUi(this);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->radioCreate);
    group->addButton(ui->radioLoad);

    ui->radioLoad->setAutoExclusive(true);
    ui->radioCreate->setAutoExclusive(true);
    connect(ui->radioLoad, SIGNAL(clicked(bool)), this, SLOT(onLoadRadioSelected(bool)));
    connect(ui->radioCreate, SIGNAL(clicked(bool)), this, SLOT(onCreateRadioSelected(bool)));

    ui->radioCreate->setChecked(true);
    onCreateRadioSelected(true);

    connect(ui->buttonBrowsePath, SIGNAL(clicked()), this, SLOT(onLoadBrowseButtonClicked()));
}

CreateGraphDialog::~CreateGraphDialog()
{
    delete ui;
}


void CreateGraphDialog::onLoadBrowseButtonClicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_graphpath", QDir::homePath()).toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Import Graph file"), lastpath);
    if (!fn.isEmpty()) {
        QString graphpath, coordspath;

        QFileInfo info(fn);
        QString fnn = info.absoluteFilePath();

        ui->loadGraphPath->setText(fnn);
    }
}

void CreateGraphDialog::onLoadRadioSelected(bool checked)
{
    if (checked) {
        ui->radioCreate->setChecked(false);
    }
    updateView();
}

void CreateGraphDialog::updateView() const
{
    auto loadEnabled = ui->radioLoad->isChecked();
    ui->frameLoad->setEnabled(loadEnabled);

    ui->frameCreate->setEnabled(!loadEnabled);
    ui->step->setEnabled(!loadEnabled);
    ui->step1->setEnabled(!loadEnabled);
    ui->step2->setEnabled(!loadEnabled);
}

void CreateGraphDialog::onCreateRadioSelected(bool checked)
{
    if (checked) {
        ui->radioLoad->setChecked(false);
    }
    updateView();
}

double CreateGraphDialog::defaultStep() const
{
    return ui->step->value();
}

double CreateGraphDialog::step1() const
{
    return ui->step1->value();
}

double CreateGraphDialog::step2() const
{
    return ui->step2->value();
}

double CreateGraphDialog::minLon() const
{
    return ui->long1->value();
}

double CreateGraphDialog::maxLon() const
{
    return ui->long2->value();
}

double CreateGraphDialog::minLat() const
{
    return ui->lat1->value();
}

double CreateGraphDialog::maxLat() const
{
    return ui->lat2->value();
}

GraphBuilder::Type CreateGraphDialog::getType() const
{
    if (ui->quad->isChecked())
        return GraphBuilder::Quad;
    else if (ui->hex->isChecked())
        return GraphBuilder::Hex;
    else if (ui->quad_planar->isChecked())
        return GraphBuilder::QuadTrivial;
    else
        return GraphBuilder::HexTrivial;
}

void CreateGraphDialog::setShapefileList(QStringList list)
{
    ui->shapefileIncluding1->addItems(list);
    ui->shapefileIncluding2->addItems(list);
    ui->shapefileExcluding->addItems(list);
}

QString CreateGraphDialog::getIncludingSelectedShapefile1() const
{
    return ui->shapefileIncluding1->currentText();
}

QString CreateGraphDialog::getIncludingSelectedShapefile2() const
{
    return ui->shapefileIncluding2->currentText();
}


QString CreateGraphDialog::getExcludingSelectedShapefile() const
{
    return ui->shapefileExcluding->currentText();
}

bool CreateGraphDialog::isOutsideEnabled() const
{
    return ui->enableOutside->isChecked();
}

bool CreateGraphDialog::isRemoveLongEdgesEnabled() const
{
    return ui->removesLongEdges->isChecked();
}

bool CreateGraphDialog::isRemoveEdgesInExclusionZoneEnabled() const
{
    return ui->removeEdgesExclusion->isChecked();
}

double CreateGraphDialog::removeLongEdgesLimit() const
{
    return ui->longEdgesLimit->value();
}

bool CreateGraphDialog::isMaxLinksEnabled() const
{
    return ui->maxLinksEnabled->isChecked();
}

bool CreateGraphDialog::isMinLinksEnabled() const
{
    return ui->minLinksEnabled->isChecked();
}

int CreateGraphDialog::getMaxLinks() const
{
    return ui->maxLinks->value();
}

int CreateGraphDialog::getMinLinks() const
{
    return ui->minLinks->value();
}

void CreateGraphDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        if (isCreateEnabled()) {
            if (std::abs(ui->lat1->value() - ui->lat2->value()) < 1e-5 ||
                std::abs(ui->long1->value() - ui->long2->value()) < 1e-5 ||
                ui->step->value() < 0.5) {
                QMessageBox::warning(this, tr("Invalid values"), tr("The fields contain some invalid value."));
            } else {
                QDialog::done(r);
            }
        } else {
            if (loadGraphPath().isEmpty() || !QFile::exists(loadGraphPath())) {
                QMessageBox::warning(this, tr("Invalid File Path"), tr("The selected file doesn't exists or is invalid"));
                return;
            }
            QDialog::done(r);
        }
    } else {
        QDialog::done(r);
    }
}

bool CreateGraphDialog::isCreateEnabled() const
{
    return ui->radioCreate->isChecked();
}

bool CreateGraphDialog::isLoadEnabled() const
{
    return ui->radioLoad->isChecked();
}

QString CreateGraphDialog::loadGraphPath() const
{
    return ui->loadGraphPath->text();
}
