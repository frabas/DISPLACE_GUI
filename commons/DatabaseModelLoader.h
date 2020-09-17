//
// Created by happycactus on 5/23/20.
//

#ifndef DISPLACE_DATABASEMODELLOADER_H
#define DISPLACE_DATABASEMODELLOADER_H


#include "utils/spimpl.h"
#include "ModelLoader.h"

namespace msqlitecpp {
namespace v2 {
class Storage;
}
}

class DatabaseModelLoader : public ModelLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    DatabaseModelLoader(std::shared_ptr<SimModel> model, std::shared_ptr<msqlitecpp::v2::Storage> db);

    void loadCalendar() override;

    void loadShips() override;

    void loadVessels(int year, int month, int quarter, int semester) override;

    void loadMetiers(int year, int month, int quarter, int semester) override;

    bool loadPopulations(int year) override;

protected:
    bool loadConfigImpl(int &nbpops, int &nbbenthospops, std::vector<int> &implicit_pops,
                        std::vector<int> &implicit_pops_level2, std::vector<int> &grouped_tacs,
                        std::vector<int> &nbcp_coupling_pops, std::vector<double> &calib_oth_landings,
                        std::vector<double> &calib_w, std::vector<double> &calib_cpue,
                        std::vector<types::NodeId> &interesting_harbours) override;

    bool loadScenarioImpl(displace::commons::Scenario &scenario) override;

    bool loadNodesAndGraphsDataImpl() override;

    bool loadFishFarmsImpl() override;

    bool loadWindmillsImpl() override;

    BenthosData
    loadBenthosData(const PopSceOptions &dyn_pop_sce, const DynAllocOptions &dyn_alloc_sce, const std::string &biolsce,
                    const std::string &fleetsce) override;
};


#endif //DISPLACE_DATABASEMODELLOADER_H
