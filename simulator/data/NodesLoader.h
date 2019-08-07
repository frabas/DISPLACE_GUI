//
// Created by happycactus on 06/08/19.
//

#ifndef DISPLACE_NODESLOADER_H
#define DISPLACE_NODESLOADER_H

#include "utils/spimpl.h"

class Node;

#include <string>
#include <vector>

namespace msqlitecpp { namespace v2 { class Storage; }}

class NodesLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit NodesLoader(std::string inputFolder, std::string folder_name_parameterization, int nrow);

    explicit NodesLoader(msqlitecpp::v2::Storage &db);

    std::vector<Node *> load(int agraph);
};


#endif //DISPLACE_NODESLOADER_H
