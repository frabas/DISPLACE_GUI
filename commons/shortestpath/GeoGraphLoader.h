//
// Created by Federico Fuga on 13/07/18.
//

#ifndef DISPLACERESEARCH_GEOGRAPHLOADER_H
#define DISPLACERESEARCH_GEOGRAPHLOADER_H

#include <commons_global.h>

#include <string>
#include <iostream>

class GeoGraph;

class COMMONSSHARED_EXPORT GeoGraphLoader {
    void loadNodes(GeoGraph &graph,std::istream &is);
    void loadEdges(GeoGraph &graph,std::istream &is);
public:
    GeoGraphLoader() = default;

    void load (GeoGraph &graph, std::string coordFileName, std::string graphFileName);
};


#endif //DISPLACERESEARCH_GEOGRAPHLOADER_H
