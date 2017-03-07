// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef SHORTESTPATHBUILDER_H
#define SHORTESTPATHBUILDER_H

#include <memory>

#include <QString>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#if defined (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/graph/dijkstra_shortest_paths.hpp>
#if defined (__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <boost/property_map/property_map.hpp>

class NodeData;
class DisplaceModel;

class ShortestPathBuilder
{
    struct flag_t {
        typedef boost::edge_property_tag kind;

        bool flag;

        flag_t() : flag(false) {}
    };

    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
        flag_t,
        boost::property < boost::edge_weight_t, double > > graph_t;
    typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef std::pair<int, int> Edge;

    DisplaceModel *mModel;

    graph_t mGraph;
    std::vector<Edge> mEdges;
    std::vector<double> mWeights;

    boost::property_map<graph_t, boost::edge_weight_t>::type mWeightmap;
    std::vector<vertex_descriptor> mPredecessors;
    std::vector<double> mDistances;

private:
    void createText (QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes);
    void createBinary (QString prev, QString mindist, const QList<std::shared_ptr<NodeData> > &relevantNodes);

public:
    explicit ShortestPathBuilder(DisplaceModel *model);

    enum Format {
        Binary, Text
    };

    void create(std::shared_ptr<NodeData> node, QString path, bool simplify, const QList<std::shared_ptr<NodeData> > &relevantNodes, Format format = Binary);
};

#endif // SHORTESTPATHBUILDER_H
