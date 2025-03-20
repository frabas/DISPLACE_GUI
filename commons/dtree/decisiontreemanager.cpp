// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "decisiontreemanager.h"

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>

#include <comstructs.h>

#include <string.h>

#if defined(_MSC_BUILD)
#include <msvc/dirent.h>
#else
#include <dirent.h>
#endif

#include <iostream>
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

DecisionTreeManager::DecisionTreeManager()
    : mTrees()
{
    for (int i = 0; i < SIZE; ++i) {
        mTrees.push_back(std::shared_ptr<dtree::DecisionTree>());
        mVariableDictionary.push_back(std::set<Variable>());
    }
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
            bool isfile = true;
#ifndef __WIN32
            isfile = dirent->d_type == DT_REG;
#endif
            if (isfile) {
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

int DecisionTreeManager::readFromScenario(std::string path, displace::commons::Scenario scenario)
{
    readScenarioFileIfAvailable(path, scenario.dt_go_fishing);
    readScenarioFileIfAvailable(path, scenario.dt_choose_ground);
    readScenarioFileIfAvailable(path, scenario.dt_start_fishing);
    readScenarioFileIfAvailable(path, scenario.dt_change_ground);
    readScenarioFileIfAvailable(path, scenario.dt_stop_fishing);
    readScenarioFileIfAvailable(path, scenario.dt_change_port);

    return 1;
}

void DecisionTreeManager::readScenarioFileIfAvailable(std::string path, std::string scenarioname)
{
    if (scenarioname.length() > 0 && scenarioname.compare(0, 1, std::string("-")) != 0) {
        if (!readFile(path + "/" + scenarioname)) {
            std::cerr << "Can't read " << scenarioname << std::endl;
            throw std::runtime_error("Can't read scenario file " +scenarioname);
        }
    }
}

struct NodePrototype {
    int id;
    Variable variable;
    double value;
    std::vector<int> children;
    std::vector<int> mapping;
    std::shared_ptr<dtree::Node> node;
};

/** \brief Loads a specific file, creating the dtree and allocating it to the type specified in the file
 * */
bool DecisionTreeManager::readFile (std::string filename)
{
//    std::cout << "@DEBUG: Reading file " << filename << std::endl;

    std::ifstream stream;
    stream.open(filename.c_str(), std::ios_base::in);
    if (stream.fail()) {
        return false;
    }

    std::shared_ptr<dtree::DecisionTree> tree (new dtree::DecisionTree());

    std::vector<NodePrototype> nodes;

    DecisionTreeManager::TreeType treeType = DecisionTreeManager::InvalidTreeType;
    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        if (stream.eof())
            break;

        boost::trim(line);
        if (boost::istarts_with(line, "#Tree")) {
            std::vector<std::string> fields;
            boost::split(fields, line, boost::is_any_of(" "));
            if (fields[0] == "#TreeVersion:") {
                int v = atoi(fields[1].c_str());
                if (v != VariableNames::VERSION) {
                    std::cerr << "Incompatible version." << std::endl;
                    return false;
                }
            } else if (fields[0] == "#TreeType:") {
                treeType = treeTypeFromCode(fields[1]);
                if (treeType == InvalidTreeType) {
                    std::cerr << "Invalid TreeType: '" << fields[1] << "'\n";
                    return false;
                }
            }
        } else if (boost::istarts_with(line, "#")) {
            // ignore
        } else {
            std::vector<std::string> fields;
            boost::split(fields, line, boost::is_any_of(","));

            NodePrototype prt;
            prt.id = atoi(fields[0].c_str());
            prt.variable = VariableNames::variableCode(fields[1]);
            // fields 2 and 3 are ignored (posx,posy)
            int n = atoi(fields[4].c_str());
            int fldnum = 5;
//            std::cout << prt.id << " " << n << " ";
            for (int i = 0; i < n; ++i) {
                // read the child
                int ch;
                std::string fld = fields[fldnum++];
                if (fld.empty()) {
                    ch = -1;
                } else {
                    ch = atoi(fld.c_str());
                }
                prt.children.push_back(ch);

//                std::cout << " chld: " << ch << " map: ";
                // read the map
                fld = fields[fldnum++];
                if (fld.empty()) {
                    ch = -1;
                } else {
                    ch = atoi(fld.c_str());
                }
                prt.mapping.push_back(ch);
//                std::cout << ch << " ";
            }
            prt.value = atof(fields[5+n].c_str());
//            std::cout << " value " << prt.value << std::endl;
            prt.node = std::shared_ptr<dtree::Node>(new dtree::Node(tree));

            while (nodes.size() <= (size_t)prt.id)
                nodes.push_back(NodePrototype());
            nodes[prt.id] = prt;
        }
    }

//    std::cout << "@DEBUG: TreeType = " << treeType << std::endl;
    if (treeType == DecisionTreeManager::InvalidTreeType) {
        std::cerr << "Invalid Tree Type" << std::endl;
        return false;
    }

    // setup the tree
    tree->setType(treeType);
    std::set<Variable> &dictionary = mVariableDictionary.at(static_cast<int>(treeType));

    if (nodes.size() > 0) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::shared_ptr<dtree::Node> n = nodes[i].node;
            n->setVariable(nodes[i].variable);
            dictionary.insert(nodes[i].variable);
            n->setValue(nodes[i].value);
            int nch = 0;
            for (size_t j = 0; j < nodes[i].children.size(); ++j) {
                int chld = nodes[i].children[j];
                if (chld != -1) {
                    n->setChild(nch++, nodes[chld].node);
                    nodes[chld].node->setParent(n);
                }
                int map = nodes[i].mapping[j];
                if (map != -1) {
                    n->setMapping(j, map);
                }
            }
        }

        /*
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::shared_ptr<dtree::Node> n = nodes[i].node;
            for (int i = 0; i < n->getChildrenCount() ; ++i) {
                std::cout << i << " unm " << n->getUnmappedChild(i).get() << " map "
                          << n->getMapping(i) << " "
                          << n->getChild(i).get();
                if (n->getUnmappedChild(i) != nullptr)
                    std::cout << " value " << n->getUnmappedChild(i)->value();
                std::cout << std::endl;
            }

        }
        */

        // set the root
        tree->setRoot(nodes[0].node);
    }

    mTrees[static_cast<int>(treeType)] = tree;

    return true;
}

/** \brief Returns a reference to the specified tree
 */
std::shared_ptr<dtree::DecisionTree> DecisionTreeManager::tree(DecisionTreeManager::TreeType type)
{
    return mTrees[static_cast<int>(type)];
}

bool DecisionTreeManager::hasTree(DecisionTreeManager::TreeType type)
{
    return tree(type).get() != 0;
}

bool DecisionTreeManager::hasTreeVariable(DecisionTreeManager::TreeType type, Variable variable) const
{
    const std::set<Variable> &dictionary = mVariableDictionary.at(static_cast<int>(type));
    std::set<Variable>::const_iterator it = dictionary.find(variable);
    return it != dictionary.end();
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
    return InvalidTreeType;
}
