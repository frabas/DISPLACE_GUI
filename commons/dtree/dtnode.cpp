// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <dtree/dtnode.h>

using namespace dtree;

Node::Node(std::shared_ptr<DecisionTree> node)
    : mTree(node),
      mNodes(),
      mExtra(),
      mVariable(VarUndefined),
      mValue(0.0)
{
}

int Node::getChildrenCount() const
{
    return mNodes.size();
}

std::shared_ptr<Node> Node::getChild(int idx)
{
    return mNodes.at(mGroups[idx]);
}

std::shared_ptr<Node> Node::getUnmappedChild(int idx)
{
    return mNodes.at(idx);
}

void Node::setChild(int idx, std::shared_ptr<Node> child)
{
    mNodes[idx] = child; /*VariableNames::Bin*/
}

void Node::setMapping(int idx, int remapped)
{
    if (idx != remapped)
        mNodes[idx].reset();
    mGroups[idx] = remapped;
}

int Node::getMapping(int idx) const
{
    return mGroups[idx];
}

void Node::setExtra(std::shared_ptr<NodeExtra> extra)
{
    mExtra = extra;
}

std::shared_ptr<NodeExtra> Node::extra() const
{
    return mExtra;
}

void Node::setVariable(Variable var)
{
    if (mVariable != var) {
        mNodes.clear();
        mGroups.clear();
        // Note: binary node
        for (int i = 0; i < VariableNames::variableBinCount(var); ++i) {
            mNodes.push_back(std::shared_ptr<Node> ());
            mGroups.push_back(i);
        }
        mVariable = var;
    }
}
