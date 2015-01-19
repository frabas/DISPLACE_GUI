#include "shortestpathbuilderworker.h"
#include <shortestpathbuilder.h>

using namespace displace::workers;

ShortestPathBuilderWorker::ShortestPathBuilderWorker(MainWindow *main, WaitDialog *dialog, DisplaceModel *model)
    : BackgroundWorkerWithWaitDialog(main, dialog), mModel(model)
{
}

void ShortestPathBuilderWorker::setRelevantNodes(const QList<std::shared_ptr<NodeData> > &nodes)
{
    mRelevantNodes = nodes;
}

void ShortestPathBuilderWorker::execute()
{
    ShortestPathBuilder builder(mModel);

    setText("Building shortest paths");
    setProgressMax(mRelevantNodes.size());
    setAbortEnabled(true);
    int n = 0;
    foreach (std::shared_ptr<NodeData> node, mRelevantNodes) {
        if (aborted()) {
            setFail(tr("Aborted by user"));
            break;
        }
        setProgress(n);

        builder.create(node, mModel->linkedShortestPathFolder());
        ++n;
    }
    setProgress(n);
}

