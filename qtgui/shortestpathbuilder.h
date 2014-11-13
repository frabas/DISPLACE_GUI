#ifndef SHORTESTPATHBUILDER_H
#define SHORTESTPATHBUILDER_H

#include <memory>

#include <QString>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

class NodeData;
class DisplaceModel;

class ShortestPathBuilder
{
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
      boost::no_property, boost::property < boost::edge_weight_t, double > > graph_t;
    typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef std::pair<int, int> Edge;

    DisplaceModel *mModel;

    graph_t g;
    std::vector<Edge> mEdges;
    std::vector<double> mWeights;

    boost::property_map<graph_t, boost::edge_weight_t>::type weightmap;
    std::vector<vertex_descriptor> p;
    std::vector<int> d;
    vertex_descriptor s;

public:
    explicit ShortestPathBuilder(DisplaceModel *model);

    void create(std::shared_ptr<NodeData> node, QString path);
};

#endif // SHORTESTPATHBUILDER_H
