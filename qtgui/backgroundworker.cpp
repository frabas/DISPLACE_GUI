// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "backgroundworker.h"

#include <mainwindow.h>
#include <waitdialog.h>

#include <QDebug>

BackgroundWorker::BackgroundWorker(MainWindow *main) :
    QObject(0),
    mMain(main),
    mResult(true),
    mError()
{
}

void BackgroundWorker::process()
{
    emit workStarted();

    execute();

    emit workEnded();
    emit completed(mResult);
}



BackgroundWorkerWithWaitDialog::BackgroundWorkerWithWaitDialog(MainWindow *main, WaitDialog *dialog)
    : BackgroundWorker(main),
      mWaitDialog(dialog),
      mAborted(false)
{
    connect (this, SIGNAL(progress(int)), mWaitDialog, SLOT(setProgression(int)));
    connect (this, SIGNAL(totalChanged(int)), mWaitDialog, SLOT(setTotal(int)));
    connect (this, SIGNAL(messageChanged(QString)), mWaitDialog, SLOT(setText(QString)));
    connect (this, SIGNAL(progressBarVisibilityChanged(bool,int)), mWaitDialog, SLOT(setProgress(bool,int)));
    connect (this, SIGNAL(abortButtonVisibilityChanged(bool)), mWaitDialog, SLOT(enableAbort(bool)));
    connect (mWaitDialog, SIGNAL(aborted()), this, SLOT(abortIssued()));

    dialog->setOnAbortListener(this);
}

void BackgroundWorkerWithWaitDialog::abortIssued()
{
    mAborted = true;
}

void BackgroundWorkerWithWaitDialog::setProgressMax(int max)
{
    emit progressBarVisibilityChanged(true, max);
}

void BackgroundWorkerWithWaitDialog::setProgress(int level)
{
    emit progress(level);
}

void BackgroundWorkerWithWaitDialog::setText(QString text)
{
    emit messageChanged(text);
}

void BackgroundWorkerWithWaitDialog::setAbortEnabled(bool en)
{
    emit abortButtonVisibilityChanged(en);
}
