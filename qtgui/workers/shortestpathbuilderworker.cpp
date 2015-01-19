#include "shortestpathbuilderworker.h"

#include <waitdialog.h>

#include <QtConcurrent>
#include <QFuture>

using namespace displace::workers;

ShortestPathBuilderWorker::ShortestPathBuilderWorker(QObject *main, WaitDialog *dialog, DisplaceModel *model)
    : QObject(main),
      mWaitDialog(dialog),
      mModel(model),
      mFutureWatcher(),
      mBuilder(mModel)
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
    mWaitDialog->setProgress(true, mRelevantNodes.size());
    mWaitDialog->enableAbort(true);
    mWaitDialog->show();

    QFuture<void> future = QtConcurrent::map(mRelevantNodes, doStep);

    mFutureWatcher.setFuture(future);
    connect (&mFutureWatcher, SIGNAL(finished()), this, SLOT(completed()));
    connect (&mFutureWatcher, SIGNAL(canceled()), this, SLOT(cancelled()));
    connect (this, SIGNAL(finished(bool)), obj, slot);
    connect (&mFutureWatcher, SIGNAL(progressValueChanged(int)), mWaitDialog, SLOT(setProgression(int)));
}

void ShortestPathBuilderWorker::doStep(arg a)
{
    a.me->mBuilder.create(a.node, a.me->mModel->linkedShortestPathFolder());
}

void ShortestPathBuilderWorker::completed()
{
    mWaitDialog->close();
    emit finished(true);
}

void ShortestPathBuilderWorker::cancelled()
{
    mWaitDialog->close();
    emit finished(false);
}

