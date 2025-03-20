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

#include "shortestpathbuilder.h"

#include "algo/ShortestPathAlgorithm.h"
#include "algo/DijkstraShortestPath.h"
#include "algo/AStarShortestPath.h"

#include <modelobjects/nodedata.h>
#include <displacemodel.h>

#include <memory>

#include <QList>
#include <QFile>
#include <QTextStream>


#include <legacy/binarygraphfilereader.h>
#include <legacy/binarygraphfilewriter.h>

/* Note
 * Part of this code was extracted from:
 * http://www.boost.org/doc/libs/1_57_0/libs/graph/example/dijkstra-example.cpp
 * */

struct ShortestPathBuilder::Impl {
    DisplaceModel *mModel;

    std::unique_ptr<ShortestPathAlgorithm> algo;

    void createText(QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes)
    {
        QFile mindist_file(mindist);
        QFile prev_file (prev);

        mindist_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        prev_file.open(QIODevice::WriteOnly | QIODevice::Truncate);

        QTextStream strm_min(&mindist_file);
        QTextStream strm_prev(&prev_file);

        strm_prev << " key  value" << endl;
        strm_min << " key  value" << endl;

        vector<int> mem(2, 0);

        vector <int> relevant_nodes;
        foreach (std::shared_ptr<NodeData> n, relevantNodes) {
            relevant_nodes.push_back(n->get_idx_node().toIndex());
        }

        algo->saveRelevantNodes(relevantNodes,[&strm_prev, &strm_min](types::NodeId node, types::NodeId pred, double dist){
            strm_prev << node.toIndex() << " " << pred.toIndex() << endl;
            strm_min << node.toIndex() << " " << dist << endl;
        });

        mindist_file.close();
        prev_file.close();
    }

    void createBinary(QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes)
    {
        displace::formats::legacy::BinaryGraphFileWriter<uint16_t,uint16_t> wr_prev;
        wr_prev.open(prev.toStdString());

        displace::formats::legacy::BinaryGraphFileWriter<uint16_t,uint16_t> wr_md;
        wr_md.open(mindist.toStdString());

        algo->saveRelevantNodes(relevantNodes,[&wr_prev, &wr_md](types::NodeId node, types::NodeId pred, double dist){
            wr_prev.write(node.toIndex(), pred.toIndex());
            wr_md.write(node.toIndex(), dist);
        });

        wr_md.close();
        wr_prev.close();
    }

    explicit Impl(DisplaceModel *model, ShortestPathBuilder::AlgoType type)
    {
        switch (type) {
            case ShortestPathBuilder::AlgoType ::AStar:
                algo = std::make_unique<AStarShortestPath>(model);
                break;
            case ShortestPathBuilder::AlgoType ::Dijkstra:
                algo = std::make_unique<DijkstraShortestPath>(model);
                break;
            default:
                throw std::logic_error("Unhandled switch case: ShortestPathBuilder::AlgoType");
        }
    }
};


ShortestPathBuilder::ShortestPathBuilder(DisplaceModel *model, AlgoType type)
        : p(std::make_unique<Impl>(model, type))
{
}

ShortestPathBuilder::~ShortestPathBuilder() noexcept = default;

void ShortestPathBuilder::create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                                 const QList<std::shared_ptr<NodeData> > &relevantNodes, Format format)
{
    Q_UNUSED(simplify);

    p->algo->create(node, path, simplify, relevantNodes);

    /*
    for(auto filter : postProcessingFilter) {
        filter(relevantNodes, mGraph, mPredecessors, mDistances);
    }
*/
    QString ext = "bin";
    if (format == Text)
        ext = "dat";

    QString mindist = QString("%1/min_distance_%2.%3").arg(path).arg(node->get_idx_node().toIndex()).arg(ext);
    QString prev = QString("%1/previous_%2.%3").arg(path).arg(node->get_idx_node().toIndex()).arg(ext);

    switch (format) {
        case Binary:
            p->createBinary(prev, mindist, relevantNodes);
            break;
        case Text:
            p->createText(prev,mindist, relevantNodes);
            break;
        default:
            throw std::runtime_error("Unhandled case");
    }
}

#if 0
void ShortestPathBuilder::appendPostProcessingFilter(ShortestPathBuilder::PostProcessingFilter filter)
{
    postProcessingFilter.push_back(filter);
}
#endif
