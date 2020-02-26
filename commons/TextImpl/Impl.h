//
// Created by happycactus on 2/22/20.
//

#ifndef DISPLACE_TextfileModelLoader_IMPL_H
#define DISPLACE_TextfileModelLoader_IMPL_H

#include "TextfileModelLoader.h"

#include <string>

struct TextfileModelLoader::Impl {
    std::string folder_name_parameterization;
    std::string inputfolder;
    std::string scenarioname;
};

#endif //DISPLACE_IMPL_H
