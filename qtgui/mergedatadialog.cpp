#include "mergedatadialog.h"
#include "ui_mergedatadialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

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

    QString tpl = QString("%") + QString::number(mOutRequiresTemplate);
    if (mOutRequiresTemplate > 0 && !ui->fileOut->text().contains(tpl)) {
        QMessageBox::warning(this, tr("Bad Output filename"),
                             QString(tr("Output file names must be a template, i.e. it requires %%1 to %%2 placeholder for progressive numbers"))
                             .arg(1).arg(mOutRequiresTemplate));
        return;
    }

    lastDistance = ui->distance->value();
    accept();
}

void MergeDataDialog::on_browseIn_clicked()
{
    QSettings set;
    QString dir = set.value("MergeDataIn.path", QDir::homePath()).toString();

    if (!ui->fileIn->text().isEmpty())
        dir = ui->fileIn->text();

    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Input file name"),
                                                dir, tr("Text files (*.txt *.dat);;All files (*.*)"));

    if (!file.isEmpty()) {
        ui->fileIn->setText(file);
        if (ui->fileOut->text().isEmpty() && mDefaultOutToIn)
            ui->fileOut->setText(file);

        QFileInfo info (file);
        set.setValue("MergeDataIn.path", info.path());

        inputFileSelected();
    }
}


void MergeDataDialog::on_browseOut_clicked()
{
    QSettings set;
    QString dir = set.value("MergeDataOut.path", QDir::homePath()).toString();

    if (!ui->fileOut->text().isEmpty())
        dir = ui->fileOut->text();

    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Output file name"),
                                                dir, tr("Text files (*.txt *.dat);;All files (*.*)"));

    if (!file.isEmpty()) {
        QString tpl = QString("%") + QString::number(mOutRequiresTemplate);
        if (mOutRequiresTemplate > 0 && !file.contains(tpl)) {
            int r = QMessageBox::question(this, tr("Template is required"),
                                          tr("A template for multiple files is required. Do you want me to append the template placeholder to the selected file name?"),
                                          QMessageBox::Yes, QMessageBox::No);
            if (r == QMessageBox::Yes) {
                QFileInfo info(file);
                QString path = info.path();
                QString name = info.fileName();

                int pos = name.lastIndexOf(".");
                name = name.left(pos) + "-%1.%2" + name.mid(pos);

                file = path + QDir::separator() + name;
            }
        }

        ui->fileOut->setText(file);

        QFileInfo info (file);
        set.setValue("MergeDataOut.path", info.path());
    }
}

void MergeDataDialog::insertPluggableComponent(QWidget *widget)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    ui->componentContainer->setLayout(layout);

    layout->addWidget(widget);
}
