// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "waitdialog.h"
#include "ui_waitdialog.h"

#include <QDebug>
#include <QApplication>
#include <QThread>

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog),
    mAbortListener(0)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    ui->setupUi(this);

    setFixedSize(size());   /* Set a fixed size, unresizeable window */
    ui->progress->setVisible(false);
    ui->progress->setValue(0);
    ui->cmdAbort->setVisible(false);
    ui->progressLabel->setVisible(false);

    connect (this, SIGNAL(self_set_progression(int)), this, SLOT(setProgression(int)), Qt::QueuedConnection);
}

WaitDialog::~WaitDialog()
{
    delete ui;
}

void WaitDialog::setFormat(QString format)
{
    ui->progress->setFormat(format);
}

void WaitDialog::setText(QString text)
{
    ui->text->setText(text);
}

void WaitDialog::setProgress(bool shown, int max)
{
    ui->progress->setVisible(shown);
    ui->progress->setMinimum(0);
    ui->progress->setMaximum(max);
}

void WaitDialog::setProgress(int pos, int max)
{
    ui->progress->setVisible(true);
    ui->progress->setMinimum(pos);
    ui->progress->setMaximum(max);
}

void WaitDialog::setTotal(int max)
{
    if (!ui->progress->isVisible())
        ui->progress->setVisible(true);
    ui->progress->setMinimum(0);
    ui->progress->setMaximum(max);
}

void WaitDialog::enableAbort(bool enable)
{
    if (enable)
        ui->cmdAbort->show();
    else
        ui->cmdAbort->hide();
}

void WaitDialog::setProgression(int level)
{
    if (QApplication::instance()->thread() != QThread::currentThread())
        emit self_set_progression(level);
    else
        ui->progress->setValue(level);
}

void WaitDialog::setEta(QString txt)
{
    if (!ui->progressLabel->isVisible())
        ui->progressLabel->setVisible(true);
    ui->progressLabel->setText(txt);
}

void WaitDialog::on_cmdAbort_clicked()
{
    qDebug() << "ABORT";
    if (mAbortListener)
        mAbortListener->abortIssued();
    emit aborted();
}
