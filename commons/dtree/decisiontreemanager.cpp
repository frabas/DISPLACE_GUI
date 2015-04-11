#include "decisiontreemanager.h"

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

}

/** \brief Returns a reference to the specified tree
 */
dtree::DecisionTree *DecisionTreeManager::tree(DecisionTreeManager::TreeType)
{

}
