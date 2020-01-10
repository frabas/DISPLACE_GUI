//
// Created by happycactus on 12/28/19.
//

#include "TextfileModelLoader.h"

#include "dataloaderbenthos.h"

#include "readdata.h"

struct TextfileModelLoader::Impl {
    std::string folder_name_parameterization;
    std::string inputfolder;
    std::string scenarioname;
};

TextfileModelLoader::TextfileModelLoader(std::shared_ptr<SimModel> model,
                                         std::string folder_name_parameterization,
                                         std::string inputfolder,
                                         std::string scenarioname)
        : ModelLoader(model),
          p(spimpl::make_unique_impl<Impl>())
{
    p->folder_name_parameterization = folder_name_parameterization;
    p->inputfolder = inputfolder;
    p->scenarioname = scenarioname;
}

ModelLoader::BenthosData TextfileModelLoader::loadBenthosData(
        PopSceOptions const &dyn_pop_sce,
        DynAllocOptions const &dyn_alloc_sce,
        std::string const &biolsce,
        std::string const &fleetsce
)
{
    Dataloaderbenthos bl;
    bl.features(p->folder_name_parameterization,
                p->inputfolder,
                dyn_pop_sce,
                dyn_alloc_sce,
                biolsce,
                fleetsce);

    return bl.loadedData();
}

bool TextfileModelLoader::loadConfigImpl(int &nbpops,
                                         int &nbbenthospops,
                                         std::vector<int> &implicit_pops,
                                         std::vector<int> &implicit_pops_level2,
                                         std::vector<int> &grouped_tacs,
                                         std::vector<int> &nbcp_coupling_pops,
                                         std::vector<double> &calib_oth_landings,
                                         std::vector<double> &calib_w,
                                         std::vector<double> &calib_cpue,
                                         std::vector<types::NodeId> &interesting_harbours)
{
    return read_config_file(nullptr,
                            p->folder_name_parameterization,
                            p->inputfolder,
                            nbpops,
                            nbbenthospops,
                            implicit_pops,
                            implicit_pops_level2,
                            grouped_tacs,
                            nbcp_coupling_pops,
                            calib_oth_landings,
                            calib_w,
                            calib_cpue,
                            interesting_harbours
    );
}

bool TextfileModelLoader::loadScenarioImpl(displace::commons::Scenario &scenario)
{
    return read_scenario_config_file(nullptr,
                                     p->folder_name_parameterization,
                                     p->inputfolder,
                                     p->scenarioname,
                                     scenario);
}
