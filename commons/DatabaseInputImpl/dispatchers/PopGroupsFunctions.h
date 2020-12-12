//
// Created by happycactus on 12/12/20.
//

#ifndef DISPLACE_POPGROUPSFUNCTIONS_H
#define DISPLACE_POPGROUPSFUNCTIONS_H

#include "DatabaseInputImpl/PopulationsLoader.h"

void
fillInitComcat(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void
fillInitPopN(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void fillInitM(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void fillInitFec(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void
fillInitWeight(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void
fillInitMaturity(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value);
void fillInitPropRecru(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                       double value);

void fillPercentAgePerSzGroup(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                              double value);

void fillPercentSzGroupPerAge(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                              double value);

void fillPropMigrants(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                      double value);

#endif //DISPLACE_POPGROUPSFUNCTIONS_H
