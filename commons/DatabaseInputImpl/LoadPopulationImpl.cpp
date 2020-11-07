//
// Created by fuga on 01/06/2020.
//



#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Population.h"

#include "PopulationsLoader.h"

bool DatabaseModelLoader::loadPopulations(int year)
{
    PopulationsLoader loader(*p->db);
    auto pops = loader.loadPopulationBaseData();

    auto populations = vector<Population *>(model().config().nbpops);
    std::transform(pops.begin(), pops.end(), std::back_inserter(populations),
                   [](PopulationsLoader::PopulationData const &d) {
                       auto pop = new Population;

                       // TODO fill pop with data from d, use std::move whenever possibile.

                       return pop;
                   });

    model().setPopulations(std::move(populations));
    return true;
}

