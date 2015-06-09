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
    void discover_vertex(Vertex u, const Graph & g) const {
        nodes.erase(u);
    }

    template < typename Vertex, typename Graph >
    void finish_vertex(Vertex u, const Graph & g) const {
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
            edges.push_back(std::make_pair(i, node->getAdiacencyByIdx(j)->target.lock()->get_idx_node()));
        }
        vis.addNode(i);
    }

    graph_t g(edges.begin(), edges.end(), mModel->getNodesCount());

    boost::breadth_first_search (g, 0, visitor(vis));

    for (std::set<int>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        qDebug() << *it;
    }

    return nodes.size() != 0;
}

