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

#include "isolatedsubgraphchecker.h"

#include <displacemodel.h>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <vector>
#include <set>

#include <utility>

//template <typename NodeSet>
class BFVisitor : public boost::default_bfs_visitor {

    std::set<int> &nodes;
public:
    BFVisitor(std::set<int> &set)
        : nodes(set) {
    }

    template <typename Node>
    void addNode (Node nd) {
        nodes.insert(nd);
    }

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph &) const {
        nodes.erase(u);
    }

    template < typename Vertex, typename Graph >
    void finish_vertex(Vertex, const Graph &) const {
    }
};

IsolatedSubgraphChecker::IsolatedSubgraphChecker(DisplaceModel *model)
    : mModel(model)
{

}

IsolatedSubgraphChecker::~IsolatedSubgraphChecker()
{

}

bool IsolatedSubgraphChecker::process()
{
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS> graph_t;
    typedef std::pair<int, int> Edge;

    std::set<int> nodes;
    BFVisitor  vis(nodes);
    std::vector<Edge> edges;

    for (int i = 0; i < mModel->getNodesCount(); ++i) {
        std::shared_ptr<NodeData> node = mModel->getNodesList().at(i);

        for (int j = 0; j < node->getAdiacencyCount(); ++j) {
            edges.push_back(std::make_pair(i, node->getAdiacencyByIdx(j)->target.lock()->get_idx_node().toIndex()));
        }
        vis.addNode(i);
    }

    graph_t g(edges.begin(), edges.end(), mModel->getNodesCount());

    boost::breadth_first_search (g, 0, visitor(vis));

    mIsolatedNodes.clear();
    for (std::set<int>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        mIsolatedNodes.push_back(*it);
    }

    return nodes.size() != 0;
}

