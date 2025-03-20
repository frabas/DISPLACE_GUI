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

#include "csveditor.h"
#include "ui_csveditor.h"

#include <csv/csvimporter.h>
#include <csv/csvexporter.h>

#include <QtConcurrent>
#include <QFileDialog>
#include <QSettings>
#include <QActionGroup>
#include <QMessageBox>

CsvEditor::CsvEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CsvEditor),
    mModel(0),
    mData(),
    mFilename(),
    mWorkLoading(),
    mWorkLoadingWatcher(),
    mWaitDialog(0),
    mImporter()
{
    ui->setupUi(this);

    QActionGroup *grp = new QActionGroup(this);
    grp->addAction(ui->actionSpace);
    grp->addAction(ui->actionComma);
    grp->addAction(ui->actionSemi_colon);
    connect (grp, SIGNAL(triggered(QAction*)), this, SLOT(separatorChanged(QAction *)));

    ui->actionComma->setChecked(true);

    ui->toolBar->addAction(ui->menuOptions->menuAction());
    ui->toolBar->addAction(ui->menuFieldSeparator->menuAction());

    mModel = new CsvTableModel(0, this);
    ui->table->setModel(mModel);

    QSettings set;
    restoreGeometry(set.value("CsvEditor.mainGeometry").toByteArray());
    restoreState(set.value("CsvEditor.mainState").toByteArray());

    updateCheckState(set.value("CsvEditor.headersShown", false).toBool());

    connect (&mWorkLoadingWatcher, SIGNAL(finished()), this, SLOT(onLoadFinished()));
}

CsvEditor::~CsvEditor()
{
    delete ui;
}

void CsvEditor::onLoadFinished()
{
    try {
        mWaitDialog->close();
        QList<QStringList> res = mWorkLoading.result();

        mData = std::shared_ptr<QList<QStringList>>( new QList<QStringList>(res));
        mModel->setSource(mData);

        QSettings set;
        QFileInfo info(mFilename);

        updateCheckState(set.value("CsvEditor.headersShown", false).toBool());

        setWindowTitle(QString(tr("CsvEditor - %1 in %2", "1: filename 2: path"))
                       .arg(info.fileName())
                       .arg(info.path()));
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Cannot load csv file"), x.what());
    }

}

void CsvEditor::separatorChanged(QAction *)
{
    QChar sep = mSeparator;

    if (ui->actionComma->isChecked())
        mSeparator = ',';
    else if (ui->actionSpace->isChecked())
        mSeparator = ' ';
    else if (ui->actionSemi_colon->isChecked())
        mSeparator = ';';

    if (sep != mSeparator && !mFilename.isEmpty()) {
        int res = QMessageBox::question(this, tr("Separator has changed"),
                                      tr("Do you want to reload the current file?"),
                                      QMessageBox::Yes, QMessageBox::No);

        if (res == QMessageBox::Yes) {
            load (mFilename);
        }
    }
}

void CsvEditor::on_action_Open_triggered()
{
    QSettings set;
    QString dir = set.value("CsvEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("CsvEditor.filter", "").toString();

    QString file = QFileDialog::getOpenFileName(this, tr("Open CSV file"),
                                                dir, tr("CSV Files (*.csv);;Text Files (*.txt);;Dat Files (*.dat);;All Files (*.*)"),
                                                &filter);

    if (!file.isEmpty()) {
        mFilename = file;

        load(mFilename);

        QFileInfo info(file);
        set.setValue("CsvEditor.LastPath", info.absoluteFilePath());
        set.setValue("CsvEditor.filter", filter);
    }
}

void CsvEditor::on_actionFirst_line_as_Headers_toggled(bool checked)
{
    if (mModel) {
        mModel->setFirstLineHeaders(checked);
    }

    QSettings set;
    if (set.value("CsvEditor.headersShown", false).toBool() != checked)
        set.setValue("CsvEditor.headersShown", checked);
}

void CsvEditor::closeEvent(QCloseEvent *)
{
    QSettings sets;
    sets.setValue("CsvEditor.mainGeometry", saveGeometry());
    sets.setValue("CsvEditor.mainState", saveState());
}

void CsvEditor::load(QString file)
{
    mImporter.setSeparator(mSeparator);
    mWorkLoading = QtConcurrent::run(&mImporter, &CsvImporter::import, file);
    mWorkLoadingWatcher.setFuture(mWorkLoading);

    mWaitDialog = new WaitDialog(this);
    mWaitDialog->setText(tr("Loading file..."));
    mWaitDialog->setModal(true);
    mWaitDialog->show();
    connect (mWaitDialog, SIGNAL(destroyed()), mWaitDialog, SLOT(deleteLater()));
}

void CsvEditor::updateCheckState(bool state)
{
    ui->actionFirst_line_as_Headers->setChecked(state);
}

void CsvEditor::on_action_Save_triggered()
{
    QSettings set;
    QString dir = set.value("CsvEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("CsvEditor.filter", "").toString();

    QString file = QFileDialog::getSaveFileName(this, tr("Save CSV file"),
                                                dir, tr("CSV Files (*.csv);;Text Files (*.txt);;Dat Files (*.dat);;All Files (*.*)"),
                                                &filter);

    if (!file.isEmpty()) {
        CsvExporter exporter;
        exporter.setSeparator(mSeparator);
        exporter.exportFile(file, *mData);

        QFileInfo info(file);
        set.setValue("CsvEditor.LastPath", info.absoluteFilePath());
        set.setValue("CsvEditor.filter", filter);
    }
}

void CsvEditor::on_action_Insert_row_after_triggered()
{
    int idx = ui->table->currentIndex().row();
    mModel->insertRows(idx+1, 1, QModelIndex());
}

void CsvEditor::on_action_Insert_row_at_current_position_triggered()
{
    int idx = ui->table->currentIndex().row();
    if (idx < 0) idx = 0;
    mModel->insertRows(idx, 1, QModelIndex());
}

void CsvEditor::on_action_Remove_current_row_triggered()
{
    int idx = ui->table->currentIndex().row();
    if (idx < 0) idx = 0;
    mModel->removeRows(idx, 1, QModelIndex());
}
