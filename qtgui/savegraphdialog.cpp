#include "savegraphdialog.h"
#include "ui_savegraphdialog.h"

#include <utils/mrupathmanager.h>

#include <QFileDialog>

SaveGraphDialog::SaveGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveGraphDialog)
{
    ui->setupUi(this);

    ui->optCustomize->setChecked(false);
}

SaveGraphDialog::~SaveGraphDialog()
{
    delete ui;
}

void SaveGraphDialog::on_optCustomize_toggled(bool checked)
{
    ui->graphFileName->setEnabled(checked);
    ui->coordFileName->setEnabled(checked);
    ui->areaCodeFileName->setEnabled(checked);
    ui->landscapeFileName->setEnabled(checked);
}

void SaveGraphDialog::on_graphName_textChanged(const QString &gn)
{
    ui->graphFileName->setText(QString("coord%1.dat").arg(gn));
    ui->coordFileName->setText(QString("graph%1.dat").arg(gn));
    ui->areaCodeFileName->setText(QString("code_area_for_graph%1_points.dat").arg(gn));
    ui->landscapeFileName->setText(QString("coord%1_with_landscape.dat").arg(gn));
}

void SaveGraphDialog::on_cmdBrowse_clicked()
{
    MruPathManager man;

    QString path = QFileDialog::getExistingDirectory(this, tr("Output Folder"), man.getMru(MruPathManager::GraphFolder));
    if (!path.isEmpty())
        ui->outputFolder->setText(path);
}
