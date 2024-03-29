//
// Created by Federico Fuga on 11/09/18.
//

#include "AStarShortestPath.h"

#include "displacemodel.h"

#include "GeographicLib/Geodesic.hpp"

#include <boost/property_map/property_map.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>

#include <vector>

using namespace boost;

namespace {


class MyGeoGraph {
public:
    // auxiliary types
    struct location {
        float y, x; // lat, long
    };

    using cost = float;

    using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property,
            boost::property<boost::edge_weight_t, cost> >;

    using WeightMap = boost::property_map<Graph, boost::edge_weight_t>::type;
    using vertex = Graph::vertex_descriptor;
    using edge_descriptor = Graph::edge_descriptor;
    using edge = std::pair<int, int>;

    Graph graph;
    WeightMap weightmap = get(boost::edge_weight, graph);
    std::vector<location> locations;

    MyGeoGraph() = default;

    void addNode(int node, float x, float y)
    {
        while (locations.size() <= node) {
            locations.push_back(location{});
        }
        locations[node] = {x, y};
    }

    bool addEdge(int node_from, int node_to, cost weight)
    {
        edge_descriptor e;
        bool inserted;

        std::tie(e, inserted) = add_edge(node_from, node_to, graph);
        weightmap[e] = weight;

//   std::cout << "Addedge: " << node_from << " to " << node_to << " w " << weight << " => " << inserted << "\n";

        return inserted;
    }

    size_t numNodes() const
    { return boost::num_vertices(graph); }

    size_t numEdges() const
    { return boost::num_edges(graph); }
};


struct found_goal {
};

// euclidean distance heuristic
template<class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType> {
    const GeographicLib::Geodesic &geod = GeographicLib::Geodesic::WGS84();

public:
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

    distance_heuristic(LocMap l, Vertex goal)
            : m_location(l), m_goal(goal)
    {}

    CostType operator()(Vertex u)
    {
        double d;
        geod.Inverse(m_location[m_goal].y, m_location[m_goal].x, m_location[u].y, m_location[u].x, d);
        return d;
    }

private:
    LocMap m_location;
    Vertex m_goal;
};

template<class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor {
public:
    astar_goal_visitor(Vertex goal) : m_goal(goal)
    {}

    template<class Graph>
    void examine_vertex(Vertex u, Graph &g)
    {
        if (u == m_goal) {
            throw found_goal();
        }
    }

private:
    Vertex m_goal;
};

}

struct AStarShortestPath::Impl {
    DisplaceModel *mModel;
    MyGeoGraph graph;

    struct PathSegment {
        types::NodeId n;
        float w;
    };

    std::map<types::NodeId, std::list<PathSegment>> paths;

    explicit Impl(DisplaceModel *model)
            : mModel(model)
    {

    }

    std::list<PathSegment>
    findShortestPath(MyGeoGraph::vertex from, MyGeoGraph::vertex to)
    {
        auto &bgraph = graph.graph;
        bool found = false;

        std::vector<MyGeoGraph::Graph::vertex_descriptor> p(graph.numNodes());
        std::vector<MyGeoGraph::cost> d(graph.numEdges());
        try {
            // call astar named parameter interface
            astar_search_tree
                    (bgraph, from,
                     distance_heuristic<MyGeoGraph::Graph, MyGeoGraph::cost, MyGeoGraph::location *>
                             (graph.locations.data(), to),
                     predecessor_map(make_iterator_property_map(p.begin(), boost::get(boost::vertex_index, bgraph))).
                             distance_map(
                             make_iterator_property_map(d.begin(), boost::get(boost::vertex_index, bgraph))).
                             visitor(astar_goal_visitor<MyGeoGraph::vertex>(to)));

        } catch (found_goal &x) {
            found = true;
        }

        std::list<PathSegment> result;
        if (!found) {
            return result;
        }

        for (auto v = to;; v = p[v]) {
            result.push_front(PathSegment{types::NodeId{static_cast<unsigned short>(v)}, d[v]});
            if (p[v] == v) {
                break;
            }
        }
        return result;
    }
};


AStarShortestPath::AStarShortestPath(DisplaceModel *model)
        : p(std::make_unique<Impl>(model))
{
    const QList<std::shared_ptr<NodeData> > &nodes = p->mModel->getNodesList();

    for (std::shared_ptr<NodeData> node: nodes) {
        for (int n = 0; n < node->getAdiacencyCount(); ++n) {
            std::shared_ptr<NodeData::Edge> edge = node->getAdiacencyByIdx(n);
            std::shared_ptr<NodeData> tg = edge->target.lock();
            if (tg.get() != nullptr) {
                p->graph.addNode(node->get_idx_node().toIndex(), node->get_x(), node->get_y());
                p->graph.addEdge(node->get_idx_node().toIndex(), tg->get_idx_node().toIndex(), edge->weight);
            }
        }
    }
}

AStarShortestPath::~AStarShortestPath() noexcept = default;

void AStarShortestPath::create(std::shared_ptr<NodeData> node, QString path, bool simplify,
                               const QList<std::shared_ptr<NodeData> > &relevantNodes)
{
    // set relevancy for relevant nodes
    for (auto rnode : relevantNodes) {
        auto l = p->findShortestPath(node->get_idx_node().toIndex(), rnode->get_idx_node().toIndex());
        p->paths.insert(std::make_pair(rnode->get_idx_node(), l));
    }
}

void AStarShortestPath::saveRelevantNodes(const QList<std::shared_ptr<NodeData> > &relevantNodes,
                                          std::function<void(types::NodeId, types::NodeId, double)> writer)
{
    std::map<types::NodeId, std::pair<types::NodeId, double>> prevmap;

    for (std::shared_ptr<NodeData> n: relevantNodes) {
        auto startnode = n->get_idx_node();
        auto pnode = startnode;
        auto pathentry = p->paths.find(pnode);
        if (pathentry != p->paths.end()) {
            const auto &path = pathentry->second;
            for (auto nodeit = path.begin(); nodeit != path.end(); ++nodeit) {
                auto w = std::make_pair(types::NodeId{pnode.toIndex()}, nodeit->w);
                prevmap.insert(std::make_pair(types::NodeId{nodeit->n.toIndex()}, w));
                pnode = nodeit->n;
            }
        }
    }


    for (auto &x : prevmap) {
        writer(x.first, x.second.first, x.second.second);
    }
}
