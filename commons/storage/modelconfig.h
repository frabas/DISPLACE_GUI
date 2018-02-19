#ifndef MODELCONFIG_H
#define MODELCONFIG_H

#include <vector>

struct ModelConfig {
    int nbpops;
    int nbbenthospops;
    int szGroups;

    std::vector <int> m_implicit_pops;
    std::vector <int> m_implicit_pops_level2;
    std::vector <int> m_grouped_tacs;
    std::vector <double> m_calib_oth_landings;
    std::vector <double> m_calib_weight_at_szgroup;
    std::vector <double> m_calib_cpue_multiplier;
};



#endif // MODELCONFIG_H
