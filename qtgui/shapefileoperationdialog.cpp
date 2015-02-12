#include "shapefileoperationdialog.h"
#include "ui_shapefileoperationdialog.h"

#include <QFileDialog>
#include <utils/mrupathmanager.h>

ShapefileOperationDialog::ShapefileOperationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShapefileOperationDialog)
{
    ui->setupUi(this);
}

ShapefileOperationDialog::~ShapefileOperationDialog()
{
    delete ui;
}

void ShapefileOperationDialog::setShapefileList(QStringList list)
{
    ui->selectShapefile->addItems(list);
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
