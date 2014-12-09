#include "backgroundworker.h"

#include <mainwindow.h>
#include <waitdialog.h>

#include <QDebug>

BackgroundWorker::BackgroundWorker(MainWindow *main) :
    QObject(0),
    mMain(main)
{
}

void BackgroundWorker::process()
{
    emit workStarted();

    execute();

    emit workEnded();
}



BackgroundWorkerWithWaitDialog::BackgroundWorkerWithWaitDialog(MainWindow *main, WaitDialog *dialog)
    : BackgroundWorker(main),
      mWaitDialog(dialog),
      mAborted(false)
{
    connect (this, SIGNAL(progress(int)), mWaitDialog, SLOT(setProgression(int)));
    connect (this, SIGNAL(messageChanged(QString)), mWaitDialog, SLOT(setText(QString)));
    connect (this, SIGNAL(progressBarVisibilityChanged(bool,int)), mWaitDialog, SLOT(setProgress(bool,int)));
    connect (this, SIGNAL(abortButtonVisibilityChanged(bool)), mWaitDialog, SLOT(enableAbort(bool)));
    connect (mWaitDialog, SIGNAL(aborted()), this, SLOT(abortIssued()));
}

void BackgroundWorkerWithWaitDialog::abortIssued()
{
    qDebug() << "Anbort issued";
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
