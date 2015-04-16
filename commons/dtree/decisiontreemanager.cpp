#include "decisiontreemanager.h"

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <string.h>
#include <dirent.h>
#include <fstream>

#include <boost/algorithm/string.hpp>

using namespace dtree;

const char * const DecisionTreeManager::mCodes[] = {
    "GoFishing", "ChooseGround",
    "StartFishing", "ChangeGround", "StopFishing",
    "ChoosePort",

    // this is the last
    0
};

DecisionTreeManager *DecisionTreeManager::mInstance = 0;

const int DecisionTreeManager::VERSION = 3;

DecisionTreeManager::DecisionTreeManager()
    : mTrees()
{
    for (int i = 0; i < SIZE; ++i)
        mTrees.push_back(boost::shared_ptr<dtree::DecisionTree>());
}

/** \brief Read all the dtrees in a directory, returning the number of loaded trees.
 */
int DecisionTreeManager::readFromDirectory(std::string path)
{
    DIR *dir;
    dirent *dirent;

    int nr = 0;
    dir = opendir(path.c_str());
    if (dir) {
        while ((dirent = readdir(dir))) {
            if (dirent->d_type == DT_REG) {
                if (readFile (path + "/" + std::string(dirent->d_name))) {
                    ++nr;
                } else {
                    std::cerr << "Can't read " << dirent->d_name << std::endl;
                }
            }
        }

        closedir(dir);
    } else {
        std::cerr << "Can't open " << path << std::endl;
    }
    return nr;
}

/** \brief Loads a specific file, creating the dtree and allocating it to the type specified in the file
 * */
bool DecisionTreeManager::readFile (std::string filename)
{
    std::cout << "@DEBUG: Reading file " << filename << std::endl;

    std::ifstream stream;
    stream.open(filename, std::ios_base::in);
    if (stream.fail()) {
        return false;
    }

    boost::shared_ptr<dtree::DecisionTree> tree (new dtree::DecisionTree());

    struct NodePrototype {
        int id;
        dtree::Variable variable;
        double value;
        std::vector<int> children;
        boost::shared_ptr<dtree::Node> node;
    };
    std::vector<NodePrototype> nodes;

    DecisionTreeManager::TreeType treeType = DecisionTreeManager::InvalidTreeType;
    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        if (stream.eof())
            break;

        if (boost::istarts_with(line, "#Tree")) {
            std::vector<std::string> fields;
            boost::split(fields, line, boost::is_any_of(" "));
            if (fields[0] == "#TreeVersion:") {
                int v = atoi(fields[1].c_str());
                if (v != VERSION) {
                    std::cerr << "Incompatible version." << std::endl;
                    return false;
                }
            } else if (fields[0] == "#TreeType:") {
                treeType = treeTypeFromCode(fields[1]);
            }
        } else if (boost::istarts_with(line, "#")) {
            // ignore
        } else {
            std::vector<std::string> fields;
            boost::split(fields, line, boost::is_any_of(","));

            NodePrototype prt;
            prt.id = atoi(fields[0].c_str());
            prt.variable = dtree::VariableNames::variableCode(fields[1]);
            // fields 2 and 3 are ignored (posx,posy)
            int n = atoi(fields[4].c_str());
            for (int i = 0; i < n; ++i) {
                int ch = atoi(fields[5+i].c_str());
                prt.children.push_back(ch);
            }
            prt.value = atof(fields[5+n].c_str());
            prt.node = boost::shared_ptr<dtree::Node>(new dtree::Node(tree));

            while (nodes.size() <= (size_t)prt.id)
                nodes.push_back(NodePrototype());
            nodes[prt.id] = prt;
        }
    }

    std::cout << "@DEBUG: TreeType = " << treeType << std::endl;
    if (treeType == DecisionTreeManager::InvalidTreeType) {
        std::cerr << "Invalid Tree Type" << std::endl;
        return false;
    }

    // setup the tree
    tree->setType(treeType);

    if (nodes.size() > 0) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            boost::shared_ptr<dtree::Node> n = nodes[i].node;
            n->setVariable(nodes[i].variable);
            n->setValue(nodes[i].value);
            for (size_t j = 0; j < nodes[i].children.size(); ++j) {
                int chld = nodes[i].children[j];
                n->setChild(j, nodes[chld].node);
                nodes[chld].node->setParent(n);
            }
        }

        // set the root
        tree->setRoot(nodes[0].node);
    }

    mTrees[static_cast<int>(treeType)] = tree;

    return true;
}

/** \brief Returns a reference to the specified tree
 */
boost::shared_ptr<dtree::DecisionTree> DecisionTreeManager::tree(DecisionTreeManager::TreeType type)
{
    return mTrees[static_cast<int>(type)];
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
