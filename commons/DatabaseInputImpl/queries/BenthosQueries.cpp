//
// Created by happycactus on 27/04/21.
//

#include "BenthosQueries.h"

namespace displace {
namespace db {
namespace defs {
namespace benthos {

const char *BenthosTableName = "BenthosParameters";

msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldParameter("parameter");
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldLandscapeId("landscape_id");
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldFuncGroup("funcgroup");
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPeriod("period");
msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Real> fieldValue("value");

}
}
}
}

using namespace displace::db::defs::benthos;

BenthosQueries::map BenthosQueries::dispatcher;


static void fillMeanWeight(ModelLoader::BenthosData &benthos, int landscape, int funcgroup, int period, double value)
{
    benthos.meanWeightPerFuncGroupPerNode.insert(std::make_pair(funcgroup, value));
}

static void fillCarryingCap(ModelLoader::BenthosData &benthos, int landscape, int funcgroup, int period, double value)
{
    benthos.carryingCapNumberPerFuncGroupPerNode.insert(std::make_pair(funcgroup, value));
}

static void
fillRecoveryingRate(ModelLoader::BenthosData &benthos, int landscape, int funcgroup, int period, double value)
{
    benthos.recoveryRatePerFuncGroupPerNode.insert(std::make_pair(funcgroup, value));
}

static void fillPropOnHab(ModelLoader::BenthosData &benthos, int landscape, int funcgroup, int period, double value)
{
    benthos.propPerFuncGroupPerNode.insert(std::make_pair(funcgroup, value));
}

BenthosQueries::BenthosQueries(msqlitecpp::v2::Storage &_db, int period)
        : db(_db),
          selectQuery(db,
                      BenthosTableName,
                      fieldParameter,
                      fieldLandscapeId,
                      fieldFuncGroup,
                      fieldPeriod,
                      fieldValue)
{
    if (period != 0) {
        msqlitecpp::v2::WhereStatement ws(
                (fieldPeriod == "0"), "OR", (msqlitecpp::v2::WhereStatement(fieldPeriod.name(), "is", "null")));
        selectQuery.where(ws);
        selectQuery.bind(period);
    }

    if (dispatcher.empty()) {
        dispatcher["MeanWeight"] = &fillMeanWeight;
        dispatcher["CarryingCap"] = &fillCarryingCap;
        dispatcher["RecoveryRate"] = &fillRecoveryingRate;
        dispatcher["PropOnHab"] = &fillPropOnHab;
    }
}

void BenthosQueries::dispatch(ModelLoader::BenthosData &benthos, std::string parameter, int landscape, int funcgroup,
                              int period, double value)
{
    auto function = dispatcher.find(parameter);
    if (function != dispatcher.end()) {
        (function->second)(benthos, landscape, funcgroup, period, value);
    } else {
        std::cout << "** Warning, can't load Benthos Parameter " << parameter << ": not implemented.\n";
    }

}

void BenthosQueries::execute(ModelLoader::BenthosData &benthos)
{
    selectQuery.execute(
            [this, &benthos](std::string parameter, int landscape, int funcgroup, int period, double value) {
                dispatch(benthos, parameter, landscape, funcgroup, period, value);
                return true;
            });
}
