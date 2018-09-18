//
// Created by Federico Fuga on 11/09/18.
//

#include "DijkstraShortestPath.h"

#include "displacemodel.h"

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

#include <vector>

namespace {
struct flag_t {
    typedef boost::edge_property_tag kind;

    bool flag;
    bool isRelevant;

    flag_t() : flag(false)
    {}
};

using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
        flag_t, boost::property<boost::edge_weight_t, double> >;
using vertex_descriptor = boost::graph_traits<graph_t>::vertex_descriptor;
using Edge = std::pair<int, int>;

/*
    using PostProcessingFilter = std::function<bool(const QList<std::shared_ptr<NodeData> > &relNodes,
                                                    graph_t &graph,
                                                    std::vector<vertex_descriptor> &predecessors,
                                                    std::vector<double> &dinstances)>;
*/

}

struct DijkstraShortestPath::Impl {
    DisplaceModel *mModel;

    graph_t mGraph;
    std::vector<Edge> mEdges;
    std::vector<double> mWeights;

    boost::property_map<graph_t, boost::edge_weight_t>::type mWeightmap;
    std::vector<vertex_descriptor> mPredecessors;
    std::vector<double> mDistances;

//    std::list<PostProcessingFilter> postProcessingFilter;

    explicit Impl(DisplaceModel *model)
            : mModel(model)
    {

    }

};

DijkstraShortestPath::DijkstraShortestPath(DisplaceModel *model)
        : p(std::make_unique<Impl>(model))
{
    const QList<std::shared_ptr<NodeData> > &nodes = p->mModel->getNodesList();

            foreach (std::shared_ptr<NodeData> node, nodes) {
            for (int n = 0; n < node->getAdiacencyCount(); ++n) {
                std::shared_ptr<NodeData::Edge> edge = node->getAdiacencyByIdx(n);
                std::shared_ptr<NodeData> tg = edge->target.lock();
                if (tg.get() != nullptr) {
                    p->mEdges.push_back(Edge(node->get_idx_node().toIndex(), tg->get_idx_node().toIndex()));
                    p->mWeights.push_back(edge->weight);
                }
            }
        }

    p->mGraph = graph_t(p->mEdges.begin(), p->mEdges.end(), p->mWeights.begin(), nodes.size());
    p->mWeightmap = get(boost::edge_weight, p->mGraph);
    p->mPredecessors = std::vector<vertex_descriptor>(num_vertices(p->mGraph));
    p->mDistances = std::vector<double>(num_vertices(p->mGraph));
}

DijkstraShortestPath::~DijkstraShortestPath() noexcept = default;

void DijkstraShortestPath::create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                                  const QList<std::shared_ptr<NodeData> > &relevantNodes)
{
    // reset relevancy of all nodes
    for (auto e = boost::vertices(p->mGraph); e.first != e.second; ++e.first) {
        p->mGraph[*e.first].isRelevant = false;
    }

    // set relevancy for relevant nodes
    for (auto rnode : relevantNodes) {
        vertex_descriptor nd = vertex(rnode->get_idx_node().toIndex(), p->mGraph);
        p->mGraph[nd].isRelevant = true;
    }

    vertex_descriptor s;

    s = vertex(node->get_idx_node().toIndex(), p->mGraph);
    dijkstra_shortest_paths(p->mGraph, s,
                            predecessor_map(boost::make_iterator_property_map(p->mPredecessors.begin(),
                                                                              get(boost::vertex_index, p->mGraph))).
                                    distance_map(boost::make_iterator_property_map(p->mDistances.begin(),
                                                                                   get(boost::vertex_index,
                                                                                       p->mGraph))));


}

void DijkstraShortestPath::saveRelevantNodes(const QList<std::shared_ptr<NodeData> > &relevantNodes,
                                             std::function<void(types::NodeId, types::NodeId, double)> writer)
{
    for (std::shared_ptr<NodeData> n: relevantNodes) {
        vertex_descriptor nd = vertex(n->get_idx_node().toIndex(), p->mGraph);

        while (p->mPredecessors[nd] != nd) {
            if (!p->mGraph[nd].flag) {
                writer(types::NodeId(nd), types::NodeId(p->mPredecessors[nd]), p->mDistances[nd]);
            }

            p->mGraph[nd].flag = true;
            nd = p->mPredecessors[nd];
        }

        p->mGraph[nd].flag = true;
    }
}
