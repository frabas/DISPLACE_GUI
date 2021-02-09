//
// Created by happycactus on 12/12/20.
//

#include "PopulationGroupsQuery.h"

#include "DatabaseInputImpl/dispatchers/PopGroupsFunctions.h"

using namespace displace::db::defs;

PopulationGroupsQuery::map PopulationGroupsQuery::dispatcher;

PopulationGroupsQuery::PopulationGroupsQuery(msqlitecpp::v2::Storage &_db, int period)
        : db(_db),
          selectQuery(db,
                      PopulationTableNameSzAg,
                      fieldSpecies,
                      fieldParameter,
                      fieldSzGroup,
                      fieldAge,
                      fieldNode,
                      fieldPeriod,
                      fieldValue)
{
    if (period != 0) {
        selectQuery.where(fieldPeriod == 0);
        selectQuery.bind(period);
    }

    if (dispatcher.empty()) {
        // beta
        dispatcher["comcat"] = &fillInitComcat;
        dispatcher["init_M"] = &fillInitM;
        dispatcher["init_fec"] = &fillInitFec;
        dispatcher["init_mat"] = &fillInitMaturity;
        dispatcher["init_popN"] = &fillInitPopN;
        dispatcher["init_weight"] = &fillInitWeight;
        dispatcher["percent_age_per_szgroup"] = &fillPercentAgePerSzGroup;
        dispatcher["percent_szgroup_per_age"] = &fillPercentSzGroupPerAge;
        dispatcher["propmigrants"] = &fillPropMigrants;
        dispatcher["proprecru"] = &fillInitPropRecru;
        //static_avai
    }
}

void PopulationGroupsQuery::dispatch(PopulationsLoader::PopulationData &population, std::string parameter,
                                     int group, int age, int node, int period,
                                     double value)
{
    auto function = dispatcher.find(parameter);
    if (function != dispatcher.end()) {
        (function->second)(population, group, age, node, period, value);
    } else {
        std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
    }
}

void PopulationGroupsQuery::execute(vector<PopulationsLoader::PopulationData> &population)
{
    selectQuery.execute(
            [this, &population](int id, std::string parameter, int group, int age, int node, int period,
                                double value) {
                dispatch(population[id], parameter, group, age, node, period, value);
                return true;
            });
}

PopulationsLoader::PopulationData PopulationGroupsQuery::executeForNamePeriod(std::string popname, int period)
{
    PopulationsLoader::PopulationData p;

    selectQuery.where(fieldPopName == "popname" && fieldPeriod == "period");
    selectQuery.bind(popname, period);
    selectQuery.execute([this, &p](int id, std::string parameter, int group, int age, int node, int period,
                                   double value) {
        dispatch(p, parameter, group, age, node, period, value);
        return true;
    });

    return p;
}
