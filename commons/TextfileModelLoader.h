//
// Created by happycactus on 12/28/19.
//

#ifndef DISPLACE_TEXTFILEMODELLOADER_H
#define DISPLACE_TEXTFILEMODELLOADER_H

#include "utils/spimpl.h"
#include "ModelLoader.h"

class PopSceOptions;

class DynAllocOptions;

class TextfileModelLoader : public ModelLoader {
public:
    TextfileModelLoader(std::shared_ptr<SimModel> model,
                        std::string folder_name_parameterization,
                        std::string inputfolder, std::string scenarioname);

protected:
    bool loadConfigImpl(int &nbpops,
                        int &nbbenthospops,
                        std::vector<int> &implicit_pops,
                        std::vector<int> &implicit_pops_level2,
                        std::vector<int> &grouped_tacs,
                        std::vector<int> &nbcp_coupling_pops,
                        std::vector<double> &calib_oth_landings,
                        std::vector<double> &calib_w,
                        std::vector<double> &calib_cpue,
                        std::vector<types::NodeId> &interesting_harbours) override;

    bool loadScenarioImpl(displace::commons::Scenario &scenario) override;

    bool loadNodesAndGraphsDataImpl() override;

    void loadShips() override;

    bool loadFishFarmsImpl() override;

    bool loadWindmillsImpl() override;

public:
    void loadCalendar() override;

protected:
    BenthosData loadBenthosData(PopSceOptions const &dyn_pop_sce,
                                DynAllocOptions const &dyn_alloc_sce,
                                std::string const &biolsce,
                                std::string const &fleetsce) override;

private:
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
};


#endif //DISPLACE_TEXTFILEMODELLOADER_H
