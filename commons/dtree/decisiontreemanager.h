#ifndef DECISIONTREEMANAGER_H
#define DECISIONTREEMANAGER_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace dtree {
class DecisionTree;

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

    /** \brief An enumerated type to identify which tree to refer.
     * They are internally used as index to various container classes (mostly vectors) so don't change
     * the integer values, especially for InvalidTreeType and SIZE (that must be the last).
     */
    enum TreeType {
        InvalidTreeType = -1,   // this is a special code, do not change this value

        GoFishing, ChooseGround,
        StartFishing, ChangeGround, StopFishing,
        ChoosePort,

        SIZE
    };

    int readFromDirectory(std::string path);
    boost::shared_ptr<dtree::DecisionTree> tree (TreeType type);

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

protected:
    bool readFile(std::string filename);

private:
    std::vector<boost::shared_ptr<dtree::DecisionTree> > mTrees;

    static DecisionTreeManager *mInstance;

    static const char * const mCodes[];
};

} // ns

#endif // DECISIONTREEMANAGER_H
