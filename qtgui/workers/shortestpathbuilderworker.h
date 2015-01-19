#ifndef SHORTESTPATHBUILDERWORKER_H
#define SHORTESTPATHBUILDERWORKER_H

#include <QObject>
#include <QFutureWatcher>
#include <shortestpathbuilder.h>

#include <backgroundworker.h>
#include <displacemodel.h>

class WaitDialog;

namespace displace {
namespace workers {

class ShortestPathBuilderWorker : public QObject
{
    Q_OBJECT

public:
    explicit ShortestPathBuilderWorker(QObject *main, WaitDialog *dialog, DisplaceModel *model);
    virtual ~ShortestPathBuilderWorker() {}

    void setRelevantNodes (const QList<std::shared_ptr<NodeData> > &nodes);
    void run(QObject *obj, const char *slot);

protected:
    struct arg {
        std::shared_ptr<NodeData> node;
        ShortestPathBuilderWorker *me;
    };

    static void doStep(arg);

signals:
    void finished (bool result);

private slots:
    void completed();
    void cancelled();

private:
    WaitDialog *mWaitDialog;
    DisplaceModel *mModel;
    QList<arg> mRelevantNodes;
    QFutureWatcher<void> mFutureWatcher;
    ShortestPathBuilder mBuilder;
};

} // workers
} // displace

#endif // SHORTESTPATHBUILDERWORKER_H
