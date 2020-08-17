//
// Created by fuga on 01/06/2020.
//



#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Population.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace db = msqlitecpp::v2;

namespace {
static const char *PopsTableName = "Populations";

db::Column<db::ColumnTypes::Integer> fieldPopId{"id"};
db::Column<db::ColumnTypes::Text> fieldPopName{"name"};
db::Column<db::ColumnTypes::Integer> fieldPopBioSce{"biosce"};

static const char *PopParamTableName = "PopulationParameters";

db::Column<db::ColumnTypes::Integer> fieldPopParId{"pop_id"};
db::Column<db::ColumnTypes::Text> fieldPopParParam{"parameter"};
db::Column<db::ColumnTypes::Text> fieldPopParValue{"value"};
db::Column<db::ColumnTypes::Text> fieldPopParCountry{"value"};

static const char *PopParamGroupTableName = "PopulationParametersWithSizeGroupAndAge";

db::Column<db::ColumnTypes::Integer> fieldPopParSzId{"pop_id"};
db::Column<db::ColumnTypes::Text> fieldPopParSzParam{"parameter"};
db::Column<db::ColumnTypes::Real> fieldPopParSzValue{"value"};
db::Column<db::ColumnTypes::Integer> fieldPopSzGroup{"size_group"};
db::Column<db::ColumnTypes::Integer> fieldPopSzAge{"age"};
db::Column<db::ColumnTypes::Integer> fieldPopSzPeriod{"period"};
db::Column<db::ColumnTypes::Integer> fieldPopSzNode{"node"};

/*
class Helper {
    sql::Storage &db;

    db::SelectStatement<db::Column<db::ColumnTypes::Integer>,
            db::Column<db::ColumnTypes::Text>> selectPop;

    db::SelectStatement<db::Column<db::ColumnTypes::Integer>,
            db::Column<db::ColumnTypes::Text>,
            db::Column<db::ColumnTypes::Real>,
            db::Column<db::ColumnTypes::Text>> selectPopParam;

public:
    Helper(sql::Storage &db_)
            : db(db_),
              selectPop(db, PopsTableName, fieldPopId, fieldPopName),
              selectPopParam(db, PopParamTableName,
                             fieldPopParId, fieldPopParParam, fieldPopParValue, fieldPopParCountry)
    {
    }


};
*/

}


bool DatabaseModelLoader::loadPopulations(int year)
{
    auto populations = vector<Population *>(model().config().nbpops);
    vector<double> global_quotas_uptake;

    auto selectPop = db::makeSelectStatement(*p->db, PopsTableName, fieldPopId, fieldPopName);
    selectPop.execute([this, &populations](int id, std::string name) {
        while (id >= populations.size()) {
            populations.push_back(nullptr);
        }
        auto p = new Population();
        p->set_name(id);
        p->set_pop_name(std::move(name));
        populations[id] = p;

        return true;
    });

    auto selectPopPar = db::makeSelectStatement(*p->db, PopParamTableName,
                                                fieldPopParId, fieldPopParValue);
    selectPopPar.where(fieldPopParParam == "param");

    selectPopPar.bind("hyperstability_param");
    selectPopPar.execute([&populations](int id, std::string val) {
        populations[id]->set_hyperstability_param(std::stod(val));
        return true;
    });
    // TODO add al other parameters here. bind each other new parameter.

    model().setPopulations(std::move(populations));
    return true;
}

