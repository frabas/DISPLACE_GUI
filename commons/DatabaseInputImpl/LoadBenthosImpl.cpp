//
// Created by happycactus on 27/04/21.
//

#include "DatabaseModelLoader.h"
#include "Impl.h"

ModelLoader::BenthosData
DatabaseModelLoader::loadBenthosData(const PopSceOptions &dyn_pop_sce, const DynAllocOptions &dyn_alloc_sce,
                                     const std::string &biolsce, const std::string &fleetsce)
{
    BenthosData data;

    if (dyn_pop_sce.option(Options::modelBenthosInN)) {
        data.int1 = 1; // is_benthos_in_numbers
    } else {
        data.int1 = 0; // is_benthos_in_numbers // if not N then it impacts the benthos biomass by default
    }

    if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
        data.int2 = 1; //is_benthos_in_longevity_classes
    } else {
        data.int2 = 0; // is_benthos_in_longevity_classes
    }


    return BenthosData();
}
