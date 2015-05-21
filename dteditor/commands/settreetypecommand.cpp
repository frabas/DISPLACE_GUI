#include "settreetypecommand.h"

#include <dteditorwindow.h>
#include <dtree/decisiontree.h>

SetTreeTypeCommand::SetTreeTypeCommand(DtEditorWindow *win, boost::shared_ptr<dtree::DecisionTree> tree, dtree::DecisionTreeManager::TreeType var)
    : mMain(win), mTree(tree), mNewType(var)
{
    mOldType = mTree->type();
}

SetTreeTypeCommand::~SetTreeTypeCommand()
{
}

bool SetTreeTypeCommand::execute()
{
    mTree->setType(mNewType);
    mMain->updateGui();
    return true;
}

bool SetTreeTypeCommand::undo()
{
    mTree->setType(mOldType);
    mMain->updateGui();
    return true;
}

bool SetTreeTypeCommand::redo()
{
    mTree->setType(mNewType);
    mMain->updateGui();
    return true;
}
