//
// Created by fuga on 02/11/2019.
//

#ifndef DISPLACE_VESSELSLOADER_H
#define DISPLACE_VESSELSLOADER_H


#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

class VesselsLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit VesselsLoader(msqlitecpp::v2::Storage &db);

    std::vector<std::string> getListOfAllVessels();

    map<int, double> getInitFuelPrices();

    struct VesselData {
        std::vector<int> fground;
        std::map<int, double> fgroundFreq;
    };

    std::shared_ptr<VesselData> getVesselData(std::string vesselname, int period);

    std::vector<double> getEconomicFeature(std::string);
};


#endif //DISPLACE_VESSELSLOADER_H
