#include "backgroundworker.h"

#include <mainwindow.h>

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

