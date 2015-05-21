#ifndef SETTREETYPECOMMAND_H
#define SETTREETYPECOMMAND_H

#include <commands/command.h>
#include <dtree/variables.h>
#include <dtree/decisiontreemanager.h>
#include <boost/shared_ptr.hpp>

namespace dtree {
class DecisionTree;
}

class DtEditorWindow;

class SetTreeTypeCommand : public Command
{
public:
    SetTreeTypeCommand(DtEditorWindow *win, boost::shared_ptr<dtree::DecisionTree> tree, dtree::DecisionTreeManager::TreeType var);
    virtual ~SetTreeTypeCommand();

    virtual bool execute();
    virtual bool undo();
    virtual bool redo();

private:
    DtEditorWindow *mMain;
    boost::shared_ptr<dtree::DecisionTree> mTree;
    dtree::DecisionTreeManager::TreeType mOldType, mNewType;
};

#endif // SETTREETYPECOMMAND_H
