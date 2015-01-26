#include "mergedatadialog.h"
#include "ui_mergedatadialog.h"

#include <QFileDialog>
#include <QSettings>

double MergeDataDialog::lastDistance = 50.0;

MergeDataDialog::MergeDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MergeDataDialog)
{
    ui->setupUi(this);

    ui->distance->setValue(lastDistance);
}

MergeDataDialog::~MergeDataDialog()
{
    delete ui;
}

QString MergeDataDialog::getInputFile() const
{
    return ui->fileIn->text();
}

QString MergeDataDialog::getOutputFile() const
{
    return ui->fileOut->text();
}

double MergeDataDialog::getDistance() const
{
    return ui->distance->value();
}

QChar MergeDataDialog::separator() const
{
    if (ui->spaces->isChecked())
        return QChar(' ');
    else if (ui->semicolon->isChecked())
        return QChar(';');
    else if (ui->commas->isChecked())
        return QChar(',');

    return QChar(';');  // default
}

void MergeDataDialog::on_ok_clicked()
{
    if (ui->fileIn->text().isEmpty() || ui->fileOut->text().isEmpty())
        return;

    lastDistance = ui->distance->value();
    accept();
}

void MergeDataDialog::on_browseIn_clicked()
{
    QSettings set;
    QString dir = set.value("MergeData.path", QDir::homePath()).toString();

    if (!ui->fileIn->text().isEmpty())
        dir = ui->fileIn->text();

    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Input file name"),
                                                dir, tr("Text files (*.txt *.dat);;All files (*.*)"));

    if (!file.isEmpty()) {
        ui->fileIn->setText(file);
        if (ui->fileOut->text().isEmpty())
            ui->fileOut->setText(file);

        QFileInfo info (file);
        set.setValue("MergeData.path", info.path());
    }
}


void MergeDataDialog::on_browseOut_clicked()
{
    QSettings set;
    QString dir = set.value("MergeData.path", QDir::homePath()).toString();

    if (!ui->fileOut->text().isEmpty())
        dir = ui->fileOut->text();

    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Output file name"),
                                                dir, tr("Text files (*.txt *.dat);;All files (*.*)"));

    if (!file.isEmpty()) {
        ui->fileOut->setText(file);

        QFileInfo info (file);
        set.setValue("MergeData.path", info.path());
    }
}
