// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "dtcsvwriter.h"
#include <graphnodeitem.h>

#include <QQueue>
#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>

const int DtCsvWriter::VERSION = 7;

DtCsvWriter::DtCsvWriter()
{
}

bool DtCsvWriter::exportTree(QTextStream &stream, dtree::DecisionTree *tree, DtGraphicsScene *scene)
{
    Q_UNUSED(tree);

    QQueue<GraphNodeItem*>queue;
    QQueue<int> queueid;

    GraphNodeItem *gnode = scene->root();
    if (!gnode)
        return false;

    stream << "#TreeVersion: " << VERSION << endl;
    if (tree->type() != dtree::DecisionTreeManager::InvalidTreeType)
        stream << "#TreeType: " << QString::fromStdString(dtree::DecisionTreeManager::treeTypeCode(tree->type())) << endl;
    stream << "# id,variable,posx,posy,nchld,children...,value" << endl;

    queue.push_back(gnode);
    queueid.push_back(0);

    int id = 0, nid = 0;
    while (!queue.isEmpty()) {
        gnode = queue.front();
        queue.pop_front();
        id = queueid.front();
        queueid.pop_front();

        std::shared_ptr<dtree::Node> node = gnode->getNode();
        stream << id << "," << dtree::VariableNames::variableName(node->variable()) << ",";
        // Other data
        QPointF pos = gnode->pos();
        stream << pos.x() << "," << pos.y() << ",";
        stream << node->getChildrenCount() << ",";

        std::set<int> set;
        for (int i = 0; i < node->getChildrenCount(); ++i) {
            int mapnode = node->getMapping(i);

            std::set<int>::iterator it = set.find(mapnode);
            if (it == set.end()) {
                GraphNodeItem *chld = gnode->getChild(mapnode);
                if (chld) {
                    ++nid;
                    stream << nid;
                    queue.push_back(chld);
                    queueid.push_back(nid);
                }
                set.insert(mapnode);
            }
            stream << "," << gnode->getNode()->getMapping(i) << ",";
        }

        stream << node->value();
        stream << endl;
    }

    return true;
}
