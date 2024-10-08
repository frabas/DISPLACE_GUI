#include "shortestpathbuilderworker.h"

#include <waitdialog.h>
#include "simplenoninterestingnodesgraphsimplifier.h"

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
    mRelevantNodes = nodes;
    foreach (std::shared_ptr<NodeData> n, nodes) {
        arg a;
        a.node = n;
        a.me = this;
        mArgumentList.push_back(a);
    }
}

void ShortestPathBuilderWorker::setRelevantInterNodes(const QVector<int> &nodes)
{
    mRelevantInternNodes = nodes;
}

void ShortestPathBuilderWorker::run(QObject *obj, const char *slot)
{
    mWaitDialog->enableAbort(true);
    mWaitDialog->show();

    QFuture<void> future = QtConcurrent::map(mArgumentList, doStep);

    mFutureWatcher.setFuture(future);
    connect (&mFutureWatcher, SIGNAL(finished()), this, SLOT(completed()));
    connect (this, SIGNAL(finished(bool)), obj, slot);
    connect (&mFutureWatcher, SIGNAL(progressValueChanged(int)), mWaitDialog, SLOT(setProgression(int)));
    connect (&mFutureWatcher, SIGNAL(progressRangeChanged(int,int)), mWaitDialog, SLOT(setProgress(int,int)));
    connect (mWaitDialog, SIGNAL(aborted()), &mFutureWatcher, SLOT(cancel()));
}

void ShortestPathBuilderWorker::doStep(arg a)
{
    try {
        ShortestPathBuilder::AlgoType type;
        switch (a.me->mAlgorithmType) {
            case AlgorithmType::Astar:
                type = ShortestPathBuilder::AlgoType::AStar;
                break;
            case AlgorithmType::Dijkstra:
                type = ShortestPathBuilder::AlgoType::Dijkstra;
                break;
            default:
                throw std::logic_error("Unhandled switch case: ShortestPathBuilderWorker::AlgorithmType");
        }

        ShortestPathBuilder builder (a.me->mModel, type);
#if 0
        SimpleNonInterestingNodesGraphSimplifier simplifier;
        simplifier.setRelevantInterNodes(a.me->mRelevantInternNodes);
        builder.appendPostProcessingFilter([&simplifier](const QList<std::shared_ptr<NodeData> > &relNodes,
                                           ShortestPathBuilder::graph_t &graph,
                                           std::vector<ShortestPathBuilder::vertex_descriptor> &predecessors,
                                           std::vector<double> &distances) {
            return simplifier(relNodes, graph, predecessors, distances);
        });
#endif
        builder.create(a.node, a.me->mModel->linkedShortestPathFolder(), true, a.me->mRelevantNodes, a.me->mTextFormat ? ShortestPathBuilder::Text : ShortestPathBuilder::Binary);
    } catch (std::exception &x) {
        qDebug() << "Cannot create node " << a.node->get_idx_node().toIndex() << ":" << x.what();
    }
}

void ShortestPathBuilderWorker::completed()
{
    mWaitDialog->close();
    if (mFutureWatcher.isCanceled())
        emit finished(false);
    else
        emit finished(true);
}
