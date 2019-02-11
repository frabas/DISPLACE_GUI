// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "createshortestpathdialog.h"
#include "ui_createshortestpathdialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>

CreateShortestPathDialog::CreateShortestPathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateShortestPathDialog)
{
    ui->setupUi(this);

    ui->graphName->setValidator(new QIntValidator);
    on_graphName_textChanged("");
    on_checkAllRelevantNodes_toggled(false);
}

CreateShortestPathDialog::~CreateShortestPathDialog()
{
    delete ui;
}

void CreateShortestPathDialog::setShortestPathFolder(QString path)
{
    ui->shortestFolder->setText(path);
}

QString CreateShortestPathDialog::getShortestPathFolder() const
{
    return ui->shortestFolder->text();
}

void CreateShortestPathDialog::setRelevantNodesFolder(QString path)
{
    ui->relevantFolder->setText(path);
}

QString CreateShortestPathDialog::getRelevantNodesFolder() const
{
    return ui->relevantFolder->text();
}

void CreateShortestPathDialog::setRelevantInterNodesFolder(QString path)
{
    ui->relevantInterFolder->setText(path);
}

QString CreateShortestPathDialog::getRelevantInterNodesFolder() const
{
    return ui->relevantInterFolder->text();
}


bool CreateShortestPathDialog::isAllNodesAreRelevantChecked() const
{
    return ui->checkAllRelevantNodes->isChecked();
}

QString CreateShortestPathDialog::getOutputFolder() const
{
    return ui->outFolder->text();
}

void CreateShortestPathDialog::setOutputFolder(const QString &folder)
{
    ui->outFolder->setText(folder);
}

QString CreateShortestPathDialog::getGraphName() const
{
    return ui->graphName->text();
}

void CreateShortestPathDialog::setGraphName(const QString &name)
{
    ui->graphName->setText(name);
}

void CreateShortestPathDialog::on_browseShortestFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_spath", QDir::homePath()).toString();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select shortest path output folder") , lastpath );
    if (!path.isEmpty()) {
        setShortestPathFolder(path);
        sets.setValue("last_spath", path);
    }
}

void CreateShortestPathDialog::on_browseRelevantFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_relpath", QDir::homePath()).toString();

    QString path = QFileDialog::getOpenFileName(this, tr("Select Relevant nodes input file") , lastpath, tr("*.dat"));
    if (!path.isEmpty()) {
        setRelevantNodesFolder(path);
        sets.setValue("last_relpath", path);
    }
}

void CreateShortestPathDialog::on_browseRelevantInterFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_relpath", QDir::homePath()).toString();

    QString path = QFileDialog::getOpenFileName(this, tr("Select Relevant intermediate nodes input file") , lastpath, tr("*.dat"));
    if (!path.isEmpty()) {
        setRelevantInterNodesFolder(path);
        sets.setValue("last_relpath", path);
    }
}

void CreateShortestPathDialog::on_ok_clicked()
{
    if (!isAllNodesAreRelevantChecked() && getRelevantNodesFolder().isEmpty()) {
        QMessageBox::warning(this, tr("No nodes specified"),
                             tr("No nodes are selected. Either tick 'All Nodes are relevant' or select the relevant nodes path"));
        return;
    }


    if (!isAllNodesAreRelevantChecked()) {
        QString refpath = ui->relevantFolder->text();
        QRegExp regexp("(.*)/vesselsspe_([^/_]+)_([^/]+).dat");

        if (regexp.indexIn(refpath) == -1) {
            QMessageBox::warning(this, tr("Relevant Nodes template check failed"),
                                 tr("The relevant nodes file name should fit the template: vesselsspe_XXX_YYY.dat.\n"
                                    "Please select a compliant file instead."));
            return;

        }

    }

    accept();
}

void CreateShortestPathDialog::on_browseOutFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_spath", QDir::homePath()).toString();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select Graph output folder") , lastpath );
    if (!path.isEmpty()) {
        setOutputFolder(path);
        if (ui->shortestFolder->text().isEmpty())
            setShortestPathFolder(path);
        sets.setValue("last_spath", path);
    }
}

void CreateShortestPathDialog::on_graphName_textChanged(const QString &)
{
    ui->ok->setEnabled(!ui->graphName->text().isEmpty());
}

void CreateShortestPathDialog::on_checkAllRelevantNodes_toggled(bool)
{
    bool en = !ui->checkAllRelevantNodes->isChecked();
    ui->relevantFolder->setEnabled(en);
    ui->browseRelevantFolder->setEnabled(en);
}

bool CreateShortestPathDialog::isBinaryFormat() const
{
    return ui->formatBinary->isChecked();
}

bool CreateShortestPathDialog::isTextFormat() const
{
    return ui->formatText->isChecked();
}

bool CreateShortestPathDialog::isAStarSelected() const
{
    return ui->selectAStarAlgorithm->isChecked();
}

bool CreateShortestPathDialog::isDijkstraSelected() const
{
    return ui->selectDijkstraAlgorithm->isChecked();
}
