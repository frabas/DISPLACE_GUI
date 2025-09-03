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

#include "dtcsvreader.h"
#include <graphnodeitem.h>

#include <dtree/decisiontree.h>
#include <dtree/dtnode.h>
#include <dtgraphicsscene.h>
#include <dtree/variables.h>

#include <QQueue>
#include <QDebug>

const int DtCsvReader::VERSION = 7;

DtCsvReader::DtCsvReader()
{
}

bool DtCsvReader::readTree(QTextStream &stream, std::shared_ptr<dtree::DecisionTree> tree, DtGraphicsScene *scene)
{
    QString line;

    struct Data {
        std::shared_ptr<dtree::Node> node;
        GraphNodeItem *item;
        int parent = -1;
        QPointF position;
        QVector<int> children;
        QVector<int> mapping;
    };

    QVector<Data> data;
    bool ok;
    int version = 0;
    dtree::DecisionTreeManager::TreeType treeType = dtree::DecisionTreeManager::InvalidTreeType;

    while (!stream.atEnd()) {
        line = stream.readLine();

        if (line.startsWith('#')) {
            QStringList fields = line.split(' ');
            if (fields[0] == "#TreeVersion:") {
                version = fields[1].toInt(&ok);
                if (!ok)
                    throw std::invalid_argument("Invalid version token");
                if (version > VERSION)
                    throw std::invalid_argument("Unsupported version");
            } else if (fields[0] == "#TreeType:") {
                treeType = dtree::DecisionTreeManager::treeTypeFromCode(fields[1].toStdString());
                if (treeType == dtree::DecisionTreeManager::InvalidTreeType)
                    throw std::invalid_argument("Unsupported #TreeType token");
            }
            continue;
        }

        QStringList fields = line.split(',', Qt::KeepEmptyParts);
        int idx = fields[0].toInt(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        while(data.size() <= idx)
            data.push_back(Data());
        data[idx].node = tree->createNode();
        data[idx].item = new GraphNodeItem(data[idx].node, scene);

        dtree::Variable var = dtree::VariableNames::variableCode(fields[1].toStdString());
        data[idx].node->setVariable(var);
        data[idx].item->setVariable(var);

        double x = fields[2].toDouble(&ok);
        if (!ok) throw std::invalid_argument("invalid field");
        double y = fields[3].toDouble(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        data[idx].position = QPointF(x,y);

        int nc = fields[4].toInt(&ok);
        if (!ok) throw std::invalid_argument("invalid field");

        auto mnc = dtree::VariableNames::variableBinCount(data[idx].node->variable());

        while (data[idx].children.size() < std::min(nc,mnc)) {
            data[idx].children.push_back(-1);
            data[idx].mapping.push_back(-1);
        }

        bool ok2;
        int fldnum = 5;
        for (int i = 0; i < nc; ++i) {
            int cidx = fields[fldnum].toInt(&ok);
            ++fldnum;

            if (i < mnc) {
                ok2 = true;
                int mapn = i;
                if (version > 5) { // backward compatibility
                    mapn = fields[fldnum].toInt(&ok2);
                    ++fldnum;
                }
                if (!ok && !ok2) continue;

                if (ok) {
                   data[idx].children[i] = cidx;
                }
                if (ok2) {
                    data[idx].mapping[i] = mapn;
                }

                while (data.size() <= cidx)
                    data.push_back(Data());

                if (ok)
                    data[cidx].parent = idx;
            }
        }

        double v = fields[fldnum++].toDouble(&ok);
        if (!ok) throw std::invalid_argument("Error parsing value field");
        data[idx].node->setValue(v);
    }

    for (int di = 0; di < data.size(); ++di) {
        Data d = data[di];

        if (d.parent == -1 && !tree->isEmpty()) {
            qDebug() << di << " is orphan";
            continue;
        }

        Q_ASSERT(d.node.get());
        Q_ASSERT(d.item);
        Q_ASSERT(d.item->getNode());

        if (tree->isEmpty()) {
            tree->setRoot(d.node);
            scene->addItemAsRoot(d.item);
        } else {
            d.node->setParent(data[d.parent].node);
            scene->addItem(d.item);
        }

        for(int i = 0; i < d.children.size(); ++i) {
            int chl = d.children[i];
            if (chl != -1) { // children is valid
                if (data[chl].node == nullptr || d.node == nullptr) {
                    throw std::invalid_argument("Malformed file, some node in the tree is referred but missing");
                }

                tree->connect(data[chl].node, d.node, d.mapping[i]);
                d.item->connectAsParent(data[chl].item, d.mapping[i]);
            }
            d.node->setMapping(i,d.mapping[i]);
        }
        d.item->setPos(d.position);

        d.item->setVariable(d.node->variable());
    }

    tree->setType(treeType);

    QQueue<GraphNodeItem *> q;
    q.push_back(scene->root());
    while (!q.empty()) {
        GraphNodeItem *item = q.front();
        q.pop_front();
        if (item) {
            for (int i = 0; i < item->getChildrenCount(); ++i) {
                GraphNodeItem *n = item->getChild(i);
                if (n)
                    q.push_back(n);
            }
            item->update();
        }
    }

    return true;
}
