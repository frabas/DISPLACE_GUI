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

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <set>


using namespace dtree;

DecisionTree::DecisionTree()
    : mRoot(),
      mType(DecisionTreeManager::InvalidTreeType)
{
}

void DecisionTree::clear()
{
    mRoot.reset();
    mType = DecisionTreeManager::InvalidTreeType;
}

std::shared_ptr<Node> DecisionTree::createNode()
{
    std::shared_ptr<Node> node(new Node(shared_from_this()));

    return node;
}

void DecisionTree::setRoot(std::shared_ptr<Node> root)
{
    mRoot = root;
}

void DecisionTree::connect(std::shared_ptr<Node> node, std::shared_ptr<Node> parent, int childId)
{
    node->setParent(parent);
    parent->setChild(childId, node);
}

void DecisionTree::removeNodes(std::list<std::shared_ptr<Node> > nodes)
{
    // collect nodes to remove
    std::list<std::shared_ptr<Node> > queue;

    for (std::list<std::shared_ptr<Node> >::iterator it = nodes.begin(); it != nodes.end(); ++it)
        queue.push_back(*it);

    std::set<std::shared_ptr<Node> > nodes_to_remove;
    while (!queue.empty()) {
        std::shared_ptr<Node> node = queue.front();
        queue.pop_front();

        if (node != nullptr) {
            nodes_to_remove.insert(node);
            for (int i = 0; i < node->getChildrenCount(); ++i) {
                std::shared_ptr<dtree::Node> ch = node->getChild(i);
                if (ch) {
                    queue.push_back(ch);
                }
            }
        }
    }

    for (std::set<std::shared_ptr<Node> >::iterator it = nodes_to_remove.begin(); it != nodes_to_remove.end(); ++it) {
        std::shared_ptr<Node> node = *it;
        if (node == nullptr)
            continue;

        std::shared_ptr<Node> parent = node->parent().lock();
        if (parent) {
            for (int j = 0; j < parent->getChildrenCount(); ++j) {
                if (parent->getChild(j) == node) {
                    parent->setChild(j, std::shared_ptr<dtree::Node>());
                    break;
                }
            }
            node->setParent(std::weak_ptr<Node>());
        } else {
            mRoot.reset();
        }
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            node->setChild(i, std::shared_ptr<Node>());
        }
    }
}
