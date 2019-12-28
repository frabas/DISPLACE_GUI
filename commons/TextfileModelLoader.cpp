//
// Created by happycactus on 12/28/19.
//

#include "TextfileModelLoader.h"

#include "dataloaderbenthos.h"

struct TextfileModelLoader::Impl {
    std::string folder_name_parameterization;
    std::string inputfolder;
};

TextfileModelLoader::TextfileModelLoader(std::string folder_name_parameterization,
                                         std::string inputfolder)
        : p(spimpl::make_unique_impl<Impl>())
{
    p->folder_name_parameterization = folder_name_parameterization;
    p->inputfolder = inputfolder;
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
