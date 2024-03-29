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

    void setBinaryFormat() { mTextFormat = false; }
    void setTextFormat() { mTextFormat = true; }
    void setRelevantNodes (const QList<std::shared_ptr<NodeData> > &nodes);
    void setRelevantInterNodes(const QVector<int> &nodes);
    void run(QObject *obj, const char *slot);

    enum class AlgorithmType { Dijkstra, Astar };
    void setAlgorithmType(AlgorithmType type) {
        mAlgorithmType = type;
    }
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

private:
    WaitDialog *mWaitDialog;
    DisplaceModel *mModel;
    QList<std::shared_ptr<NodeData>> mRelevantNodes;
    QVector<int> mRelevantInternNodes;
    QList<arg> mArgumentList;
    QFutureWatcher<void> mFutureWatcher;
    AlgorithmType mAlgorithmType;

    bool mTextFormat = false;
};

} // workers
} // displace

#endif // SHORTESTPATHBUILDERWORKER_H
