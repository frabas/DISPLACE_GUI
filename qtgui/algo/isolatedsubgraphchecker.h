#ifndef ISOLATEDSUBGRAPHCHECKER_H
#define ISOLATEDSUBGRAPHCHECKER_H

#include <memory>

class DisplaceModel;

class IsolatedSubgraphChecker
{
public:
    IsolatedSubgraphChecker(DisplaceModel *model);
    ~IsolatedSubgraphChecker();

    bool process();
private:

    DisplaceModel *mModel;
};

#endif // ISOLATEDSUBGRAPHCHECKER_H
