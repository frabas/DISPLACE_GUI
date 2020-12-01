//
// Created by happycactus on 30/11/20.
//

#include "PopParametersDispatchers.h"

static void assign_array(std::vector<double> &array, int index, double value)
{
    while (array.size() < index + 1) {
        array.push_back(0);
    }
    array[index] = value;
}

void fill_fbar_min(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 0, value);
}

void fill_fbar_max(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 1, value);
}

void fill_ftarget(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 2, value);
}

void fill_fpercent(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 3, value);
}

void fill_tacpercent(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 4, value);
}

void fill_btrigger(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 5, value);
}

void fill_fmsy(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.fbar_ages_min_max, 6, value);

    // add default values for F-MSY-low and F-MSY-up
    if (population.fbar_ages_min_max.size() < 8) {
        assign_array(population.fbar_ages_min_max, 7, value);
    }
    if (population.fbar_ages_min_max.size() < 9) {
        assign_array(population.fbar_ages_min_max, 8, value);
    }
}

void fill_tac_initial(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    assign_array(population.init_tac, 0, value);
}

void fill_ssb1(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    // TODO fill ssb_r_1
}

void fill_ssb2(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    // TODO fill ssb_r_2
}

void fill_hypers(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    population.hyperstability_param = value;
}

void fill_crty(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    // TODO fill crty
}

void fill_dhab(PopulationsLoader::PopulationData &population, int period, int landscape, double value)
{
    // TODO fill DeltaHAB
}
