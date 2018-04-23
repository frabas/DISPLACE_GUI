// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <modelobjects/nodedata.h>
#include <displacemodel.h>

#include <memory>

#include <QList>

#include <vector>
#include <algorithm>    // std::find

#include <QFile>
#include <QTextStream>


#include <legacy/binarygraphfilereader.h>
#include <legacy/binarygraphfilewriter.h>

/* Note
 * Part of this code was extracted from:
 * http://www.boost.org/doc/libs/1_57_0/libs/graph/example/dijkstra-example.cpp
 * */

void ShortestPathBuilder::createText(QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes, const QVector<int> &relevantInterNodesIdx, int flag_out)
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

    foreach (std::shared_ptr<NodeData> n, relevantNodes) {
        vertex_descriptor nd = vertex(n->get_idx_node().toIndex(), mGraph);
        vertex_descriptor initial_node_idx =n->get_idx_node().toIndex();

        mem.at(0)=0;
        mem.at(1)=0;
       if(flag_out) cout << "trace the path back from nd " << nd << endl;

        while (mPredecessors[nd] != nd) {
            if (!mGraph[nd].flag) {
                std::vector<int>::iterator it;
                it = std::find (relevant_nodes.begin(), relevant_nodes.end(), nd);
                if (it != relevant_nodes.end() ) mem.at(0)=nd;

                it = find (relevant_nodes.begin(), relevant_nodes.end(), mPredecessors[nd]);
                if (it != relevant_nodes.end() && mem.at(1)==0)  mem.at(1)=mPredecessors[nd];

                // keep the node onboard if it is a significant intermediate
                int idx=relevantInterNodesIdx.indexOf(nd);
                if (idx != -1)  mem.at(0)=nd;

                idx=relevantInterNodesIdx.indexOf(mPredecessors[nd]);
                if (idx != -1  && mem.at(1)==0) mem.at(1)=mPredecessors[nd];

                if(flag_out) cout << nd << "--" << mPredecessors[nd] << endl;

                if(mem.at(0)!=0 && mem.at(1)!=0){
                   if(flag_out) cout << "export-->> " << mem.at(0) << "--" << mPredecessors[nd] << endl;
                   strm_prev << mem.at(0) << " " << mPredecessors[nd] << endl;
                   strm_min << mem.at(0) << " " << mDistances[nd] << endl;
                   mem.at(1)=0;
                  }

             }


            mGraph[nd].flag = true;
            if(flag_out && initial_node_idx==6651)  cout << "here nd is "<< nd << " and the mPredecessors[nd] is " << mPredecessors[nd] << endl;
            nd = mPredecessors[nd];
        }

        mGraph[nd].flag = true;
    }

    mindist_file.close();
    prev_file.close();
}

void ShortestPathBuilder::createBinary(QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes, const QVector<int> &relevantInterNodesIdx, int flag_out)
{
    displace::formats::legacy::BinaryGraphFileWriter<uint16_t,uint16_t> wr_prev;
    wr_prev.open(prev.toStdString());

    displace::formats::legacy::BinaryGraphFileWriter<uint16_t,uint16_t> wr_md;
    wr_md.open(mindist.toStdString());


    if(relevantInterNodesIdx.empty())
    {
     foreach (std::shared_ptr<NodeData> n, relevantNodes) {
        vertex_descriptor nd = vertex(n->get_idx_node().toIndex(), mGraph);

        while (mPredecessors[nd] != nd) {
            if (!mGraph[nd].flag) {
                wr_prev.write(nd, mPredecessors[nd]);
                wr_md.write(nd, mDistances[nd]);
            }

            mGraph[nd].flag = true;
            nd = mPredecessors[nd];
        }

        mGraph[nd].flag = true;
     }

    }
    else
    {
        vector<int> mem(2, 0);

        vector <int> relevant_nodes;
        foreach (std::shared_ptr<NodeData> n, relevantNodes) {
           relevant_nodes.push_back(n->get_idx_node().toIndex());
        }

        foreach (std::shared_ptr<NodeData> n, relevantNodes) {
           vertex_descriptor nd = vertex(n->get_idx_node().toIndex(), mGraph);

           mem.at(0)=0;
           mem.at(1)=0;
           if(flag_out) cout << "trace the path back from nd " << nd << endl;

           while (mPredecessors[nd] != nd) {
               if (!mGraph[nd].flag) {
                   std::vector<int>::iterator it;
                   it = std::find (relevant_nodes.begin(), relevant_nodes.end(), nd);
                   if (it != relevant_nodes.end() ) mem.at(0)=nd;

                   it = find (relevant_nodes.begin(), relevant_nodes.end(), mPredecessors[nd]);
                   if (it != relevant_nodes.end() && mem.at(1)==0)  mem.at(1)=mPredecessors[nd];

                   // keep the node onboard if it is a significant intermediate
                   int idx=relevantInterNodesIdx.indexOf(nd);
                   if (idx != -1)  mem.at(0)=nd;

                   idx=relevantInterNodesIdx.indexOf(mPredecessors[nd]);
                   if (idx != -1  && mem.at(1)==0) mem.at(1)=mPredecessors[nd];

                   if(flag_out) cout << nd << "--" << mPredecessors[nd] << endl;

                   if(mem.at(0)!=0 && mem.at(1)!=0){
                      if(flag_out) cout << "export-->> " << mem.at(0) << "--" << mPredecessors[nd] << endl;
                      wr_prev.write(nd, mPredecessors[nd]);
                      wr_md.write(nd, mDistances[nd]);
                      mem.at(1)=0;
                   }
               }
               mGraph[nd].flag = true;
               nd = mPredecessors[nd];
           }

           mGraph[nd].flag = true;
        }

    }



    wr_md.close();
    wr_prev.close();
}

ShortestPathBuilder::ShortestPathBuilder(DisplaceModel *model)
    : mModel (model)
{
    const QList<std::shared_ptr<NodeData> >& nodes = mModel->getNodesList();

    foreach (std::shared_ptr<NodeData> node, nodes) {
        for (int n = 0; n < node->getAdiacencyCount(); ++n) {
            std::shared_ptr<NodeData::Edge> edge = node->getAdiacencyByIdx(n);
            std::shared_ptr<NodeData> tg = edge->target.lock();
            if (tg.get() != nullptr) {
                mEdges.push_back(Edge(node->get_idx_node().toIndex(), tg->get_idx_node().toIndex()));
                mWeights.push_back(edge->weight);
            }
        }
    }

    mGraph = graph_t(mEdges.begin(), mEdges.end(), mWeights.begin(), nodes.size());
    mWeightmap = get(boost::edge_weight, mGraph);
    mPredecessors = std::vector<vertex_descriptor> (num_vertices(mGraph));
    mDistances = std::vector<double> (num_vertices(mGraph));
}

void ShortestPathBuilder::create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                                 const QList<std::shared_ptr<NodeData> > &relevantNodes, const QVector<int> &relevantInterNodesIdx, Format format)
{
    Q_UNUSED(simplify);

    vertex_descriptor s;

    s = vertex(node->get_idx_node().toIndex(), mGraph);
    dijkstra_shortest_paths(mGraph, s,
                             predecessor_map(boost::make_iterator_property_map(mPredecessors.begin(), get(boost::vertex_index, mGraph))).
                             distance_map(boost::make_iterator_property_map(mDistances.begin(), get(boost::vertex_index, mGraph))));

    QString ext = "bin";
    if (format == Text)
        ext = "dat";

    QString mindist = QString("%1/min_distance_%2.%3").arg(path).arg(node->get_idx_node().toIndex()).arg(ext);
    QString prev = QString("%1/previous_%2.%3").arg(path).arg(node->get_idx_node().toIndex()).arg(ext);

    int flag_out=0;
    if(node->get_idx_node().toIndex() ==91) flag_out= 1;

    switch (format) {
    case Binary:
        createBinary(prev, mindist, relevantNodes, relevantInterNodesIdx, flag_out);
        break;
    case Text:
        createText(prev,mindist, relevantNodes, relevantInterNodesIdx, flag_out);
        break;
    default:
        throw std::runtime_error("Unhandled case");
    }

}
