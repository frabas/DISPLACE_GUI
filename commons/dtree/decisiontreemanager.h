#ifndef DECISIONTREEMANAGER_H
#define DECISIONTREEMANAGER_H

#include <string>
#include <vector>

namespace dtree {
class DecisionTree;
}

/** \brief A class that implements a Singleton for managing the Decision Trees for vessels
 *
 * */
class DecisionTreeManager
{
private:
    DecisionTreeManager();

    /** \brief Class destructor, not implemented to prevent desctruction. */
    ~DecisionTreeManager();
public:

    /** \brief An enumerated type to identify which tree to refer. */
    enum TreeType {
        InvalidTreeType = -1,   // this is a special code, do not change this value

        GoFishing, ChooseGround,
        StartFishing, ChangeGround, StopFishing,
        ChoosePort,

        SIZE
    };

    int readFromDirectory(std::string path);
    dtree::DecisionTree *tree (TreeType);

    /** \brief returns the instance of the singleton.
     * Uses lazy initialization to proper initialize the instance.
     */
    static DecisionTreeManager *manager() {
        if (mInstance == 0)
            mInstance = new DecisionTreeManager();
        return mInstance;
    }

    static std::string treeTypeCode(TreeType type);
    static TreeType treeTypeFromCode (std::string code);

private:
    std::vector<dtree::DecisionTree *> mTrees;

    static DecisionTreeManager *mInstance;

    static const char * const mCodes[];
};

#endif // DECISIONTREEMANAGER_H
