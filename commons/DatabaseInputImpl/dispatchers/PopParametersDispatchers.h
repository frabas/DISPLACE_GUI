//
// Created by happycactus on 30/11/20.
//

#ifndef DISPLACE_POPPARAMETERSDISPATCHERS_H
#define DISPLACE_POPPARAMETERSDISPATCHERS_H

#include "DatabaseInputImpl/PopulationsLoader.h"

void fill_fbar_min(PopulationsLoader::PopulationData &population, int period, int landscape, double value);
void fill_fbar_max(PopulationsLoader::PopulationData &population, int, int, double);
void fill_ftarget(PopulationsLoader::PopulationData &population, int, int, double);
void fill_fpercent(PopulationsLoader::PopulationData &population, int, int, double);
void fill_tacpercent(PopulationsLoader::PopulationData &population, int, int, double);
void fill_btrigger(PopulationsLoader::PopulationData &population, int, int, double);
void fill_fmsy(PopulationsLoader::PopulationData &population, int, int, double);

void fill_tac_initial(PopulationsLoader::PopulationData &population, int, int, double);
void fill_ssb1(PopulationsLoader::PopulationData &population, int, int, double);
void fill_ssb2(PopulationsLoader::PopulationData &population, int, int, double);
void fill_hypers(PopulationsLoader::PopulationData &population, int, int, double);
void fill_crty(PopulationsLoader::PopulationData &population, int, int, double);
void fill_dhab(PopulationsLoader::PopulationData &population, int, int, double);

#endif //DISPLACE_POPPARAMETERSDISPATCHERS_H
