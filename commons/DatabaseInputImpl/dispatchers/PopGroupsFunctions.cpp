//
// Created by happycactus on 12/12/20.
//

#include "PopGroupsFunctions.h"

template<typename V>
V &at(std::vector<V> &v, int index)
{
    while (v.size() <= index) {
        v.emplace_back(V{});
    }
    return v.at(index);
}

void
fillInitComcat(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_comcat_at_szgroup, group) = value;
}

void fillInitPopN(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_tot_N_at_szgroup, group) = value;
}

void fillInitM(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_M_at_szgroup, group) = value;
}

void fillInitFec(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_fecundity_at_szgroup, group) = value;
}

void
fillInitWeight(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_weight_at_szgroup, group) = value;
}

void
fillInitMaturity(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_maturity_at_szgroup, group) = value;
}

void
fillInitPropRecru(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_proprecru_at_szgroup, group) = value;
}

void fillPercentAgePerSzGroup(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                              double value)
{
    // TODO check the ordering of the indexes!
    at(at(population.percent_age_per_szgroup_matrix, age), group) = value;
}

void fillPercentSzGroupPerAge(PopulationsLoader::PopulationData &population, int group, int age, int node, int period,
                              double value)
{
    // TODO check the ordering of the indexes!
    at(at(population.percent_szgroup_per_age_matrix, group), age) = value;
}

void
fillPropMigrants(PopulationsLoader::PopulationData &population, int group, int age, int node, int period, double value)
{
    at(population.init_prop_migrants_in_tot_N_at_szgroup, group) = value;
}
