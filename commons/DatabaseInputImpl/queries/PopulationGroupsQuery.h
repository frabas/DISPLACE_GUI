//
// Created by happycactus on 12/12/20.
//

#ifndef DISPLACE_POPULATIONGROUPSQUERY_H
#define DISPLACE_POPULATIONGROUPSQUERY_H

#include "PopulationTableDefs.h"

#include "DatabaseInputImpl/PopulationsLoader.h"

#include <vector>
#include <string>

class PopulationGroupsQuery {
    msqlitecpp::v2::Storage &db;
    using func = std::function<void(PopulationsLoader::PopulationData &population, int group, int age, int node,
                                    int period, double value)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;

    msqlitecpp::v2::SelectStatement<
            decltype(displace::db::defs::fieldSpecies),
            decltype(displace::db::defs::fieldParameter),
            decltype(displace::db::defs::fieldSzGroup),
            decltype(displace::db::defs::fieldAge),
            decltype(displace::db::defs::fieldNode),
            decltype(displace::db::defs::fieldPeriod),
            decltype(displace::db::defs::fieldValue)
    > selectQuery;

    void dispatch(PopulationsLoader::PopulationData &population,
                  std::string parameter, int group, int age, int node, int period,
                  double value);

public:
    explicit PopulationGroupsQuery(msqlitecpp::v2::Storage &_db);

    void execute(std::vector<PopulationsLoader::PopulationData> &population);
    PopulationsLoader::PopulationData executeForNamePeriod(std::string popname, int period);
};


#endif //DISPLACE_POPULATIONGROUPSQUERY_H
