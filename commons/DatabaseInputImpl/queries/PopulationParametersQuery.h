//
// Created by happycactus on 02/12/20.
//

#ifndef DISPLACE_POPULATIONPARAMETERSQUERY_H
#define DISPLACE_POPULATIONPARAMETERSQUERY_H

#include "PopulationTableDefs.h"

#include "DatabaseInputImpl/PopulationsLoader.h"

#include <vector>
#include <string>

class PopulationParametersQuery {
    msqlitecpp::v2::Storage &db;
    using func = std::function<void(PopulationsLoader::PopulationData &population, int, int, double)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;

    msqlitecpp::v2::SelectStatement<
            decltype(displace::db::defs::fieldSpecies),
            decltype(displace::db::defs::fieldParameter),
            decltype(displace::db::defs::fieldCountry),
            decltype(displace::db::defs::fieldPeriod),
            decltype(displace::db::defs::fieldLandscape),
            decltype(displace::db::defs::fieldValue)
    > selectQuery;

    void dispatch(PopulationsLoader::PopulationData &population,
                  std::string parameter, std::string country, int period,
                  int landscape,
                  double value);

public:
    PopulationParametersQuery(msqlitecpp::v2::Storage &_db, int period = 0);

    void execute(std::vector<PopulationsLoader::PopulationData> &population);
    PopulationsLoader::PopulationData executeForNamePeriod(std::string popname, int period);
};


#endif //DISPLACE_POPULATIONPARAMETERSQUERY_H
