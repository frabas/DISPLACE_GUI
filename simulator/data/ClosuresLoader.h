//
// Created by happycactus on 11/16/19.
//

#ifndef DISPLACE_CLOSURESLOADER_H
#define DISPLACE_CLOSURESLOADER_H

#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

struct NodeBanningInfo;

class ClosuresLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit ClosuresLoader(msqlitecpp::v2::Storage &db);

    bool read_metier_closures(int graph_spe, int period, vector<NodeBanningInfo> &ban_info);
    bool read_vsize_closures(int graph_spe, int period, vector<NodeBanningInfo> &ban_info);
};


#endif //DISPLACE_CLOSURESLOADER_H
