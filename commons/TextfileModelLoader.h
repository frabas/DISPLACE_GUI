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
    TextfileModelLoader(std::string folder_name_parameterization,
                        std::string inputfolder);

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
