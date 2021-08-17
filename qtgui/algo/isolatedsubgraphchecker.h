#ifndef ISOLATEDSUBGRAPHCHECKER_H
#define ISOLATEDSUBGRAPHCHECKER_H

#include <memory>

#include <QVector>
#include <QList>

class DisplaceModel;

class IsolatedSubgraphChecker
{
public:
    IsolatedSubgraphChecker(DisplaceModel *model);
    ~IsolatedSubgraphChecker();

    bool process();

    QList<int> getIsolatedNodes() const {
        return mIsolatedNodes;
    }

private:

    DisplaceModel *mModel;
    QList <int> mIsolatedNodes;
};

#endif // ISOLATEDSUBGRAPHCHECKER_H
