// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

void MergeDataDialog::setInputFile(QString _fileIn)
{
    ui->fileIn->setText(_fileIn);
}

void MergeDataDialog::setOutputFile(QString _fileOut)
{
    ui->fileOut->setText(_fileOut);
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

    QFileDialog dlg (this,
                     tr("Output file name"),
                     dir, tr("Text files (*.txt *.dat);;All files (*.*)"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setDefaultSuffix("txt");
    dlg.selectFile("%1spe_full_avai_szgroup_nodes_semester%2");


    if (dlg.exec() == QDialog::Accepted) {
        QString file = dlg.selectedFiles().at(0);

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
                QString ext;
                if (pos != -1) {
                    ext = name.mid(pos);
                }

                QString tname = name.left(pos) + "-";
                for (int i = 0; i < mOutRequiresTemplate; ++i) {
                    if (i > 0)
                        tname.append(".");
                   tname.append("%").append(QString::number((i+1)));
                }
                tname.append(ext);

                file = path + "/" + tname;
            }
        }

        ui->fileOut->setText(file);

        QFileInfo info (file);
        set.setValue("MergeDataOut.path", info.path());
    }
}

void MergeDataDialog::insertPluggableComponent(QWidget *widget)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    ui->componentContainer->setLayout(layout);

    layout->addWidget(widget);
}
