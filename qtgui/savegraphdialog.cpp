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

#include "savegraphdialog.h"
#include "ui_savegraphdialog.h"

#include <utils/mrupathmanager.h>

#include <QFileDialog>

SaveGraphDialog::SaveGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveGraphDialog)
{
    ui->setupUi(this);

    ui->optAreaCodes->setChecked(true);
    ui->optLandscape->setChecked(true);
    ui->optWind->setChecked(true);
    ui->optSST->setChecked(true);
    ui->optSalinity->setChecked(true);
    ui->optNitrogen->setChecked(true);
    ui->optPhosphorus->setChecked(true);
    ui->optOxygen->setChecked(true);
    ui->optDissolvedCarbon->setChecked(true);
    ui->optBenthosBio->setChecked(true);
    ui->optBenthosNb->setChecked(true);
    ui->optClosedPoly->setChecked(true);
    ui->optCustomize->setChecked(true); // force updating
    ui->optCustomize->setChecked(false);
    ui->graphName->setText("0");
}

SaveGraphDialog::~SaveGraphDialog()
{
    delete ui;
}

void SaveGraphDialog::setName(QString name)
{
    ui->graphName->setText(name);
}

void SaveGraphDialog::setOutputFolder(QString path)
{
    ui->outputFolder->setText(path);
}

QString SaveGraphDialog::getGraphFilename() const
{
    return ui->outputFolder->text() + "/" + ui->graphFileName->text();
}

QString SaveGraphDialog::getCoordsFilename() const
{
    return ui->outputFolder->text() + "/" + ui->coordFileName->text();
}

QString SaveGraphDialog::getLandscapeFilename() const
{
    if (ui->optLandscape->isChecked())
        return ui->outputFolder->text() + "/" + ui->landscapeFileName->text();
    return QString();
}

QString SaveGraphDialog::getWindFilename() const
{
    if (ui->optWind->isChecked())
        return ui->outputFolder->text() + "/" + ui->windFileName->text();
    return QString();
}

QString SaveGraphDialog::getSSTFilename() const
{
    if (ui->optSST->isChecked())
        return ui->outputFolder->text() + "/" + ui->sstFileName->text();
    return QString();
}

QString SaveGraphDialog::getSalinityFilename() const
{
    if (ui->optSalinity->isChecked())
        return ui->outputFolder->text() + "/" + ui->salinityFileName->text();
    return QString();
}

QString SaveGraphDialog::getNitrogenFilename() const
{
    if (ui->optNitrogen->isChecked())
        return ui->outputFolder->text() + "/" + ui->NitrogenFileName->text();
    return QString();
}

QString SaveGraphDialog::getPhosphorusFilename() const
{
    if (ui->optPhosphorus->isChecked())
        return ui->outputFolder->text() + "/" + ui->PhosphorusFileName->text();
    return QString();
}

QString SaveGraphDialog::getOxygenFilename() const
{
    if (ui->optOxygen->isChecked())
        return ui->outputFolder->text() + "/" + ui->OxygenFileName->text();
    return QString();
}

QString SaveGraphDialog::getDissolvedCarbonFilename() const
{
    if (ui->optDissolvedCarbon->isChecked())
        return ui->outputFolder->text() + "/" + ui->DissolvedCarbonFileName->text();
    return QString();
}


QString SaveGraphDialog::getBenthosFilename() const
{
    if (ui->optBenthosBio->isChecked())
        return ui->outputFolder->text() + "/" + ui->benthosFileName->text();
    return QString();
}


QString SaveGraphDialog::getBenthosNbFilename() const
{
    if (ui->optBenthosNb->isChecked())
        return ui->outputFolder->text() + "/" + ui->benthosNbFileName->text();
    return QString();
}

QString SaveGraphDialog::getAreacodesFilename() const
{
    if (ui->optAreaCodes->isChecked())
        return ui->outputFolder->text() + "/" + ui->areaCodeFileName->text();
    return QString();
}

QString SaveGraphDialog::getClosedPolygonFilename() const
{
    return ui->outputFolder->text() + "/" + ui->closedPolyFilename->text();
}

QString SaveGraphDialog::getClosedPolygonFilenameMonthly() const
{
    return ui->outputFolder->text() + "/" + ui->closedPolyFilename_Months->text();
}

QString SaveGraphDialog::getClosedPolygonFilenameVesSize() const
{
    return ui->outputFolder->text() + "/" + ui->closedPolyFilename_VesSize->text();
}

bool SaveGraphDialog::isClosedPolygonExportChecked() const
{
    return ui->optClosedPoly->isChecked();
}

void SaveGraphDialog::on_optCustomize_toggled(bool checked)
{
    ui->graphFileName->setEnabled(checked);
    ui->coordFileName->setEnabled(checked);
    ui->areaCodeFileName->setEnabled(checked);
    ui->landscapeFileName->setEnabled(checked);
    ui->windFileName->setEnabled(checked);
    ui->sstFileName->setEnabled(checked);
    ui->salinityFileName->setEnabled(checked);
    ui->NitrogenFileName->setEnabled(checked);
    ui->PhosphorusFileName->setEnabled(checked);
    ui->OxygenFileName->setEnabled(checked);
    ui->DissolvedCarbonFileName->setEnabled(checked);
    ui->benthosFileName->setEnabled(checked);
    ui->benthosNbFileName->setEnabled(checked);
    ui->closedPolyFilename->setEnabled(checked);
    ui->closedPolyFilename_Months->setEnabled(checked);
    ui->closedPolyFilename_VesSize->setEnabled(checked);
}

void SaveGraphDialog::on_graphName_textChanged(const QString &gn)
{
    ui->coordFileName->setText(QString("coord%1.dat").arg(gn));
    ui->graphFileName->setText(QString("graph%1.dat").arg(gn));
    ui->areaCodeFileName->setText(QString("code_area_for_graph%1_points.dat").arg(gn));
    ui->landscapeFileName->setText(QString("coord%1_with_landscape.dat").arg(gn));
    ui->windFileName->setText(QString("coord%1_with_wind.dat").arg(gn));
    ui->sstFileName->setText(QString("coord%1_with_sst.dat").arg(gn));
    ui->salinityFileName->setText(QString("coord%1_with_salinity.dat").arg(gn));
    ui->NitrogenFileName->setText(QString("coord%1_with_Nitrogen.dat").arg(gn));
    ui->PhosphorusFileName->setText(QString("coord%1_with_Phosphorus.dat").arg(gn));
    ui->OxygenFileName->setText(QString("coord%1_with_Oxygen.dat").arg(gn));
    ui->DissolvedCarbonFileName->setText(QString("coord%1_with_DissolvedCarbon.dat").arg(gn));
    ui->benthosFileName->setText(QString("coord%1_with_benthos_total_biomass.dat").arg(gn));
    ui->benthosNbFileName->setText(QString("coord%1_with_benthos_total_number.dat").arg(gn));
    ui->closedPolyFilename->setText(QString("metier_closure_a_graph%1_quarter?.dat").arg(gn));
    ui->closedPolyFilename_Months->setText(QString("metier_closure_a_graph%1_month?.dat").arg(gn));
    ui->closedPolyFilename_VesSize->setText(QString("vsize_closure_a_graph%1_month?.dat").arg(gn));
}

void SaveGraphDialog::on_cmdBrowse_clicked()
{
    MruPathManager man;

    QString path = QFileDialog::getExistingDirectory(this, tr("Output Folder"), man.getMru(MruPathManager::GraphFolder));
    if (!path.isEmpty()) {
        man.setMru(MruPathManager::GraphFolder, path);
        ui->outputFolder->setText(path);
    }
}
