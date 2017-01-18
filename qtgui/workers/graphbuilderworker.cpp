#include "graphbuilderworker.h"

GraphBuilderWorker::GraphBuilderWorker(MainWindow *win, GraphBuilder *b, WaitDialog *dlg)
    : BackgroundWorker(win),
      builder(b),
      waitDialog(dlg)
{
    connect (this, &GraphBuilderWorker::mainMessage, dlg, &WaitDialog::setText);
    connect (this, &GraphBuilderWorker::progressMessage, dlg, &WaitDialog::setEta);
}

void GraphBuilderWorker::execute()
{
    builder->setFeedback(this);
    result = builder->buildGraph();
    mMain->graphCreated(result);
}

void GraphBuilderWorker::setMax (int m)
{
    emit totalChanged(m);
}

void GraphBuilderWorker::setStep(int step)
{
    emit progress(step);
}


void GraphBuilderWorker::setMainMessage(QString msg)
{
    emit mainMessage(msg);
}

void GraphBuilderWorker::setPartMessage(QString msg)
{
    emit progressMessage(msg);
}
