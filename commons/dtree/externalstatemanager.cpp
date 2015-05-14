#include "externalstatemanager.h"

ExternalStateManager *ExternalStateManager::sInstance = 0;

ExternalStateManager::ExternalStateManager()
{
    for (int i = 0; i < dtree::Variable::VarLast; ++i)
        mStandardEvaluators.push_back(0);

    // build evaluators
//    mStandardEvaluators[dtree::Variable::fish_price] = new ...
}

dtree::StateEvaluator *ExternalStateManager::getStandardEvaluator(dtree::Variable variable) const
{
    return mStandardEvaluators[static_cast<int>(variable)];
}

ExternalStateManager *ExternalStateManager::instance()
{
    if (sInstance == 0) {
        sInstance = new ExternalStateManager();
    }

    return sInstance;
}
