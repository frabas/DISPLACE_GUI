//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_MAPGRAPH_H
#define DISPLACE_MAPGRAPH_H

#include <boost/graph/adjacency_list.hpp>

class MapGraph {
public:
    explicit MapGraph();
    ~MapGraph() noexcept = default;


private:
    boost::adjacency_list<> mGraph;
};


#endif //DISPLACE_MAPGRAPH_H
