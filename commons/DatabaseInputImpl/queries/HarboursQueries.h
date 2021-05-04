//
// Created by happycactus on 29/04/21.
//

#ifndef DISPLACE_HARBOURSQUERIES_H
#define DISPLACE_HARBOURSQUERIES_H

#include "utils/spimpl.h"
#include "idtypes.h"

#include <msqlitecpp/v2/selectstatement.h>

#include <map>

class HarboursQueries {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    HarboursQueries(msqlitecpp::v2::Storage *db);

    struct FuelPrices {
        types::NodeId node;
        int vesselSize;
        int period;
        double value;
    };
    std::multimap<types::NodeId, FuelPrices> getFuelPrices(int period);

    struct FishPrices {
        types::NodeId node;
        int marketCat;
        int species;

        int period;
        double value;
    };
    std::multimap<types::NodeId, FishPrices> getFishPrices(int period);
};


#endif //DISPLACE_HARBOURSQUERIES_H
