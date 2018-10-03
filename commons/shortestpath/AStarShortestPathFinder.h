//
// Created by Federico Fuga on 11/07/18.
//

#ifndef EXPERIMENTS_ASTARTSHORTESTPATHFINDER_H
#define EXPERIMENTS_ASTARTSHORTESTPATHFINDER_H

#include <commons_global.h>

#include "GeoGraph.h"



#include <list>

#include<idtypes.h>
typedef types::NodeId::type vertex_t;


class COMMONSSHARED_EXPORT AStarShortestPathFinder {
public:
    std::list<types::NodeId> findShortestPath (GeoGraph &graph, GeoGraph::vertex from, GeoGraph::vertex to);
};


#endif //EXPERIMENTS_ASTARTSHORTESTPATHFINDER_H
