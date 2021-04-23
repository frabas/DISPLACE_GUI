//
// Created by happycactus on 02/12/20.
//

#include "PopulationParametersQuery.h"

#include "DatabaseInputImpl/dispatchers/PopParametersDispatchers.h"

using namespace displace::db::defs;

PopulationParametersQuery::map PopulationParametersQuery::dispatcher;

PopulationParametersQuery::PopulationParametersQuery(msqlitecpp::v2::Storage &_db, int period)
        : db(_db),
          selectQuery(db,
                      PopulationTableName,
                      fieldSpecies,
                      fieldParameter,
                      fieldCountry,
                      fieldPeriod,
                      fieldLandscape,
                      fieldValue)
{
    if (period != 0) {
        msqlitecpp::v2::WhereStatement ws(
                (fieldPeriod == "0"), "OR", (msqlitecpp::v2::WhereStatement(fieldPeriod.name(), "is", "null")));
        selectQuery.where(ws);
        selectQuery.bind(period);
    }

    if (dispatcher.empty()) {
        dispatcher["fbar_min"] = &fill_fbar_min;
        dispatcher["fbar_max"] = &fill_fbar_max;
        dispatcher["ftarget"] = &fill_ftarget;
        dispatcher["fpercent"] = &fill_fpercent;
        dispatcher["TACpercent"] = &fill_tacpercent;
        dispatcher["Btrigger"] = &fill_btrigger;
        dispatcher["FMSY"] = &fill_fmsy;

        dispatcher["TACinitial"] = &fill_tac_initial;
        dispatcher["SSB_R_1"] = &fill_ssb1;
        dispatcher["SSB_R_2"] = &fill_ssb2;

        dispatcher["hyperstability_param"] = &fill_hypers;
        dispatcher["ctrysspe_relative_stability"] = &fill_crty;
        dispatcher["DeltaHab"] = &fill_dhab;
    }
}


void PopulationParametersQuery::dispatch(PopulationsLoader::PopulationData &population, std::string parameter,
                                         std::string country, int period, int landscape, double value)
{
    auto function = dispatcher.find(parameter);
    if (function != dispatcher.end()) {
        (function->second)(population, period, landscape, value);
    } else {
        std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
    }
}

void PopulationParametersQuery::execute(vector<PopulationsLoader::PopulationData> &population)
{
    selectQuery.execute(
            [this, &population](int id, std::string parameter, std::string country, int period, int landscape,
                                double value) {
                dispatch(population[id], parameter, country, period, landscape, value);
                return true;
            });
}

PopulationsLoader::PopulationData PopulationParametersQuery::executeForNamePeriod(std::string popname, int period)
{
    PopulationsLoader::PopulationData p;

    selectQuery.where(fieldPopName == "popname" && fieldPeriod == "period");
    selectQuery.bind(popname, period);
    selectQuery.execute([this, &p](int id, std::string parameter, std::string country, int period, int landscape,
                                   double value) {
        dispatch(p, parameter, country, period, landscape, value);
        return true;
    });

    return p;
}
