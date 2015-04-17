#include "settreetypecommand.h"

#include <dtree/decisiontree.h>

SetTreeTypeCommand::SetTreeTypeCommand(boost::shared_ptr<dtree::DecisionTree> tree, dtree::DecisionTreeManager::TreeType var)
    : mTree(tree), mNewType(var)
{
    mOldType = mTree->type();
}

SetTreeTypeCommand::~SetTreeTypeCommand()
{
}

bool SetTreeTypeCommand::execute()
{
    mTree->setType(mNewType);
    return true;
}

bool SetTreeTypeCommand::undo()
{
    mTree->setType(mOldType);
    return true;
}

bool SetTreeTypeCommand::redo()
{
    mTree->setType(mNewType);
    return true;
}
