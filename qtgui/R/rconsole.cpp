// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "rconsole.h"
#include "ui_rconsole.h"

#include <QFileDialog>
#include <QProcess>

RConsole::RConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RConsole)
{
    ui->setupUi(this);

    ui->result->setReadOnly(true);
}

RConsole::~RConsole()
{
    delete ui;
}

void RConsole::on_execute_clicked()
{
    try {
        QString result = "no R interfacing implemented yet.";
        ui->result->appendPlainText(result);
    } catch (std::exception &exc) {
        ui->result->appendPlainText("** ERROR: " + QString::fromStdString(exc.what()));
    }
}

void RConsole::on_action_Close_triggered()
{
    close();
}

void RConsole::on_actionExecute_triggered()
{
    QString obj = QFileDialog::getOpenFileName(this, tr("Open R script"), QString(), tr("R scripts (*.R);;All files (*)"));
    if (!obj.isEmpty()) {
        QFileInfo info(obj);

        mProcess = new QProcess;
        mProcess->setWorkingDirectory(info.absolutePath());
        connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
        connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
        QStringList args;
        args << obj;

        mProcess->start("Rscript", args);
    }
}

void RConsole::readOutput()
{
    QByteArray out = mProcess->readAllStandardOutput();

    QString txt = QString::fromLatin1(out);
    ui->result->appendPlainText(txt);
}

void RConsole::readError()
{
    QByteArray out = mProcess->readAllStandardError();

    QString txt = QString::fromLatin1(out);
    ui->result->appendHtml("<font color=\"#aa0000\">" + txt + "</font>");
}
