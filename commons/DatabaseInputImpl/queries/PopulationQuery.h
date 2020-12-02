//
// Created by happycactus on 02/12/20.
//

#ifndef DISPLACE_POPULATIONQUERY_H
#define DISPLACE_POPULATIONQUERY_H

#include "PopulationTableDefs.h"

#include <vector>
#include <string>

class PopulationAllQuery {
    msqlitecpp::v2::Storage &db;
public:
    explicit PopulationAllQuery(msqlitecpp::v2::Storage &_db) : db(_db)
    {}

    std::vector<std::pair<int, std::string>> execute()
    {
        std::vector<std::pair<int, std::string>> populations;

        auto selectAllPopulations = msqlitecpp::v2::makeSelectStatement(db,
                                                                        displace::db::defs::PopulationListTableName,
                                                                        displace::db::defs::fieldPopId,
                                                                        displace::db::defs::fieldPopName);
        selectAllPopulations.execute([&populations](int id, std::string name) {
            populations.emplace_back(std::make_pair(id, std::move(name)));
            return true;
        });

        return populations;
    }
};


#endif //DISPLACE_POPULATIONQUERY_H
