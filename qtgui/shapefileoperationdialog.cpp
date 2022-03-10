// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "shapefileoperationdialog.h"
#include "ui_shapefileoperationdialog.h"

#include <QFileDialog>
#include <utils/mrupathmanager.h>

ShapefileOperationDialog::ShapefileOperationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShapefileOperationDialog)
{
    ui->setupUi(this);

    ui->radioImportShapefile->setChecked(true);
}

ShapefileOperationDialog::~ShapefileOperationDialog()
{
    delete ui;
}

void ShapefileOperationDialog::setShapefileList(QStringList list)
{
    if (list.size() > 0) {
        ui->selectShapefile->addItems(list);
        ui->radioSelectShapefile->setChecked(true);
    }
}

QString ShapefileOperationDialog::selectedShapefile() const
{
    if (ui->radioImportShapefile->isChecked()) {
        return ui->shapefilePath->text();
    } else if (ui->radioSelectShapefile->isChecked()) {
        return ui->selectShapefile->currentText();
    } else {
        return QString();
    }
}

void ShapefileOperationDialog::on_radioSelectShapefile_toggled(bool checked)
{
    ui->selectShapefile->setEnabled(checked);
}

void ShapefileOperationDialog::on_radioImportShapefile_toggled(bool checked)
{
    ui->cmdBrowse->setEnabled(checked);
    ui->shapefilePath->setEnabled(checked);
}

void ShapefileOperationDialog::on_cmdBrowse_clicked()
{
    MruPathManager pm;
    QString file = QFileDialog::getOpenFileName(this, tr("Select shapefile"), pm.getMru(MruPathManager::ShapefileFolder),
                                                tr("Shapefiles (*.shp);;All files (*.*)")
                                                );

    if (!file.isEmpty()) {
        ui->shapefilePath->setText(file);
        pm.setMru(MruPathManager::ShapefileFolder, file);
    }
}
