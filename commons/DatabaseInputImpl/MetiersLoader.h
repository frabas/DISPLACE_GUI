
#ifndef DISPLACE_METIERSLOADER_H
#define DISPLACE_METIERSLOADER_H


#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; } }

class MetiersLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit MetiersLoader(msqlitecpp::v2::Storage& db);

    std::vector<std::string> getListOfAllMetiers();

    //map<int, double> getInitFuelPrices();

    struct MetierData {
        int type;
        double percent_revenue_completeness;
        std::vector< vector <double> > selectivity_per_stock;
        std::vector<double> betas_per_pop;
        std::vector<double> discardratio_limits;
        std::vector<int>    is_avoided_stocks;
        std::vector<int>    mls_cat_per_pop;
        double fspeed;
        double gear_width_a;
        double gear_width_b;
        string gear_width_model;
        std::multimap<int, double> loss_after_1_passage;
        std::vector<int> metier_target_stocks;
        std::vector<int> metier_suitable_seabottomtypes;
    };

    std::shared_ptr<MetierData> getMetierData(std::string metiername, int period);

};


#endif //DISPLACE_METIERSLOADER_H
