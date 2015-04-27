#ifndef EXTERNALSTATEMANAGER_H
#define EXTERNALSTATEMANAGER_H

#include <dtree/stateevaluator.h>
#include <dtree/variables.h>

#include <vector>

/** \brief A monostate/singleton class to manage External States variables
 * */
class ExternalStateManager
{
    static ExternalStateManager *sInstance;

    ExternalStateManager();
    ~ExternalStateManager();    ///< class desctructor, not implemented because the singleton is not destroyable.

public:

    dtree::StateEvaluator *getStandardEvaluator(dtree::Variable variable) const;

    static ExternalStateManager *instance();

private:
    std::vector<dtree::StateEvaluator *> mStandardEvaluators;
};

#endif // EXTERNALSTATEMANAGER_H
