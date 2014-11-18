#include "shortestpathbuilder.h"

#include <modelobjects/nodedata.h>
#include <displacemodel.h>

#include <memory>

#include <QList>
#include <QFile>
#include <QTextStream>

/* Note
 * Part of this code was extracted from:
 * http://www.boost.org/doc/libs/1_57_0/libs/graph/example/dijkstra-example.cpp
 * */

ShortestPathBuilder::ShortestPathBuilder(DisplaceModel *model)
    : mModel (model)
{
    const QList<std::shared_ptr<NodeData> >& nodes = mModel->getNodesList();

    foreach (std::shared_ptr<NodeData> node, nodes) {
        for (int n = 0; n < node->getAdiacencyCount(); ++n) {
            mEdges.push_back(Edge(node->get_idx_node(), node->getAdiacencyByIdx(n)));
            mWeights.push_back(node->getAdiacencyWeight(n));
        }
    }

    g = graph_t(mEdges.begin(), mEdges.end(), mWeights.begin(), nodes.size());
    weightmap = get(boost::edge_weight, g);
    p = std::vector<vertex_descriptor> (num_vertices(g));
    d = std::vector<int> (num_vertices(g));
}

void ShortestPathBuilder::create(std::shared_ptr<NodeData> node, QString path)
{
    s = vertex(node->get_idx_node(), g);
    dijkstra_shortest_paths(g, s,
                             predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
                             distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

    QString mindist = QString("%1/min_distance_%2.dat").arg(path).arg(node->get_idx_node());
    QString prev = QString("%1/previous_%2.dat").arg(path).arg(node->get_idx_node());
    QFile mindist_file(mindist);
    QFile prev_file (prev);

    mindist_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    prev_file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream strm_min(&mindist_file);
    QTextStream strm_prev(&prev_file);

    strm_prev << " key  value" << endl;
    strm_min << " key  value" << endl;

    boost::graph_traits < graph_t >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
        boost::graph_traits < graph_t >::edge_descriptor e = *ei;
        boost::graph_traits < graph_t >::vertex_descriptor u = source(e, g), v = target(e, g);

        strm_prev << v << " " << u << endl;
        strm_min << v << " " << get(weightmap, e) << endl;
    }


     std::ofstream dot_file(QString("%1/graph_%2.dot").arg(path).arg(node->get_idx_node()).toStdString());

     dot_file << "digraph D {\n"
       << "  rankdir=LR\n"
       << "  size=\"4,3\"\n"
       << "  ratio=\"fill\"\n"
       << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

     for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
       boost::graph_traits < graph_t >::edge_descriptor e = *ei;
       boost::graph_traits < graph_t >::vertex_descriptor
         u = boost::source(e, g), v = boost::target(e, g);
       dot_file << u << " -> " << v
         << "[label=\"" << get(weightmap, e) << "\"";
       if (p[v] == u)
         dot_file << ", color=\"black\"";
       else
         dot_file << ", color=\"grey\"";
       dot_file << "]";
     }
     dot_file << "}";

}
