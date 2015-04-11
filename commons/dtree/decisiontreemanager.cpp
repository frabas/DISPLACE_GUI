#include "decisiontreemanager.h"

#include <dtree/decisiontree.h>
#include <string.h>

const char * const DecisionTreeManager::mCodes[] = {
    "GoFishing", "ChooseGround",
    "StartFishing", "ChangeGround", "StopFishing",
    "ChoosePort",

    // this is the last
    0
};

DecisionTreeManager *DecisionTreeManager::mInstance = 0;

DecisionTreeManager::DecisionTreeManager()
    : mTrees()
{
    for (int i = 0; i < SIZE; ++i)
        mTrees.push_back(new dtree::DecisionTree());
}

/** \brief Read all the dtrees in a directory, returning the number of loaded trees.
 */
int DecisionTreeManager::readFromDirectory(std::string path)
{
    return 0;
}

/** \brief Returns a reference to the specified tree
 */
dtree::DecisionTree *DecisionTreeManager::tree(DecisionTreeManager::TreeType)
{
    return 0;
}

std::string DecisionTreeManager::treeTypeCode(DecisionTreeManager::TreeType type)
{
    return std::string(mCodes[type]);
}

DecisionTreeManager::TreeType DecisionTreeManager::treeTypeFromCode(std::string code)
{
    for (int i = 0; i < static_cast<int>(SIZE); ++i) {
        if (strcmp(code.c_str(), mCodes[i]) == 0)
            return static_cast<TreeType>(i);
    }
    return SIZE;
}
