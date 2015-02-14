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
    return ui->outputFolder->text() + "/" + ui->landscapeFileName->text();
}

QString SaveGraphDialog::getAreacodesFilename() const
{
    return ui->outputFolder->text() + "/" + ui->areaCodeFileName->text();
}

QString SaveGraphDialog::getClosedPolygonFilename() const
{
    return ui->outputFolder->text() + "/" + ui->closedPolyFilename->text();
}

void SaveGraphDialog::on_optCustomize_toggled(bool checked)
{
    ui->graphFileName->setEnabled(checked);
    ui->coordFileName->setEnabled(checked);
    ui->areaCodeFileName->setEnabled(checked);
    ui->landscapeFileName->setEnabled(checked);
    ui->closedPolyFilename->setEnabled(checked);
}

void SaveGraphDialog::on_graphName_textChanged(const QString &gn)
{
    ui->coordFileName->setText(QString("coord%1.dat").arg(gn));
    ui->graphFileName->setText(QString("graph%1.dat").arg(gn));
    ui->areaCodeFileName->setText(QString("code_area_for_graph%1_points.dat").arg(gn));
    ui->landscapeFileName->setText(QString("coord%1_with_landscape.dat").arg(gn));
    ui->closedPolyFilename->setText(QString("nodes_in_polygons_a_graph%1_quarter?.dat").arg(gn));
}

void SaveGraphDialog::on_cmdBrowse_clicked()
{
    MruPathManager man;

    QString path = QFileDialog::getExistingDirectory(this, tr("Output Folder"), man.getMru(MruPathManager::GraphFolder));
    if (!path.isEmpty())
        ui->outputFolder->setText(path);
}
