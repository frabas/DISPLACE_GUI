#ifndef SHORTESTPATHBUILDERWORKER_H
#define SHORTESTPATHBUILDERWORKER_H

#include <backgroundworker.h>
#include <displacemodel.h>

namespace displace {
namespace workers {

class ShortestPathBuilderWorker : public BackgroundWorkerWithWaitDialog
{
public:
    explicit ShortestPathBuilderWorker(MainWindow *main, WaitDialog *dialog, DisplaceModel *model);

    void setRelevantNodes (const QList<std::shared_ptr<NodeData> > &nodes);

    void execute() override;

private:
    DisplaceModel *mModel;
    QList<std::shared_ptr<NodeData> > mRelevantNodes;

};

} // workers
} // displace

#endif // SHORTESTPATHBUILDERWORKER_H
