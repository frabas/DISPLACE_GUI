#include "shortestpathbuilderworker.h"

#include <waitdialog.h>

#include <QtConcurrent>
#include <QFuture>

using namespace displace::workers;

ShortestPathBuilderWorker::ShortestPathBuilderWorker(QObject *main, WaitDialog *dialog, DisplaceModel *model)
    : QObject(main),
      mWaitDialog(dialog),
      mModel(model),
      mFutureWatcher()
{
}

void ShortestPathBuilderWorker::setRelevantNodes(const QList<std::shared_ptr<NodeData> > &nodes)
{
    foreach (std::shared_ptr<NodeData> n, nodes) {
        arg a;
        a.node = n;
        a.me = this;
        mRelevantNodes.push_back(a);
    }
}

void ShortestPathBuilderWorker::run(QObject *obj, const char *slot)
{
    mWaitDialog->enableAbort(true);
    mWaitDialog->show();

    QFuture<void> future = QtConcurrent::map(mRelevantNodes, doStep);

    mFutureWatcher.setFuture(future);
    connect (&mFutureWatcher, SIGNAL(finished()), this, SLOT(completed()));
    connect (this, SIGNAL(finished(bool)), obj, slot);
    connect (&mFutureWatcher, SIGNAL(progressValueChanged(int)), mWaitDialog, SLOT(setProgression(int)));
    connect (&mFutureWatcher, SIGNAL(progressRangeChanged(int,int)), mWaitDialog, SLOT(setProgress(int,int)));
    connect (mWaitDialog, SIGNAL(aborted()), &mFutureWatcher, SLOT(cancel()));
}

void ShortestPathBuilderWorker::doStep(arg a)
{
    ShortestPathBuilder builder (a.me->mModel);
    builder.create(a.node, a.me->mModel->linkedShortestPathFolder());
}

void ShortestPathBuilderWorker::completed()
{
    mWaitDialog->close();
    if (mFutureWatcher.isCanceled())
        emit finished(false);
    else
        emit finished(true);
}
