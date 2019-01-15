#ifndef MODELCONFIG_H
#define MODELCONFIG_H

#include "commons_global.h"

#include <vector>

struct COMMONSSHARED_EXPORT ModelConfig {
    int nbpops;
    int nbbenthospops;
    int szGroups;

    std::vector <int> m_implicit_pops;
    std::vector <double> m_calib_oth_landings;
    std::vector <double> m_calib_weight_at_szgroup;
    std::vector <double> m_calib_cpue_multiplier;
    std::vector <int> m_implicit_pops_level2;
    std::vector <int> m_grouped_tacs;
};



#endif // MODELCONFIG_H
