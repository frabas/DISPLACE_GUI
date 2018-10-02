//
// Created by Federico Fuga on 11/07/18.
//

#include "AStarShortestPathFinder.h"

#include <boost/graph/astar_search.hpp>

using namespace boost;

namespace {

struct found_goal {
};

// euclidean distance heuristic
template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
public:
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    distance_heuristic(LocMap l, Vertex goal)
            : m_location(l), m_goal(goal) {}
    CostType operator()(Vertex u)
    {
        CostType dx = m_location[m_goal].x - m_location[u].x;
        CostType dy = m_location[m_goal].y - m_location[u].y;
        return ::sqrt(dx * dx + dy * dy);
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

//std::list<int>
std::list<types::NodeId>
AStarShortestPathFinder::findShortestPath(GeoGraph &ggraph, GeoGraph::vertex from, GeoGraph::vertex to)
{
    auto &graph = ggraph.graph;

    bool found = false;

    std::vector <GeoGraph::Graph::vertex_descriptor> p(num_vertices(graph));
    std::vector <GeoGraph::cost> d(num_vertices(graph));
    try {
        // call astar named parameter interface
        astar_search_tree
                (graph, from,
                 distance_heuristic<GeoGraph::Graph, GeoGraph::cost, GeoGraph::location *>
                         (ggraph.locations.data(), to),
                 predecessor_map(make_iterator_property_map(p.begin(), get(vertex_index, graph))).
                         distance_map(make_iterator_property_map(d.begin(), get(vertex_index, graph))).
                         visitor(astar_goal_visitor<GeoGraph::vertex>(to)));

    } catch (found_goal &x) {
        found = true;
    }

    std::list<types::NodeId> result;
    if (!found)
        return result;

    for(auto v = to;; v = p[v]) {
        result.push_front(types::NodeId(static_cast<unsigned short> (v) ));
        if(p[v] == v)
            break;
    }
    return result;

}
