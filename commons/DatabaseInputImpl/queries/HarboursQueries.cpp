//
// Created by happycactus on 29/04/21.
//

#include "HarboursQueries.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace {

static const char *HarboursTableName = "HarboursSpe";
static const char *HarboursParametersTableName = "HarboursParameters";
static const char *HarboursParametersMarketTableName = "HarboursParametersWithSpeciesAndMarketCat";
static const char *HarboursParametersPerVesselSizeTableName = "HarboursParametersWithVesselSize";

static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fieldParameter("parameter");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldNodeId("node_id");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldHarbourName("harbour_name");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldPeriod("period");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldVesselSize("vesselsize");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Real> fieldValue("value");

}

namespace sql = msqlitecpp::v2;

struct HarboursQueries::Impl {
    msqlitecpp::v2::Storage *db;

    Impl(msqlitecpp::v2::Storage *b) : db(b)
    {}
};

HarboursQueries::HarboursQueries(msqlitecpp::v2::Storage *db)
        : p(spimpl::make_unique_impl<Impl>(db))
{
}

class HarboursVesselsTableQueries {
    sql::Storage *db;

    sql::SelectStatement<decltype(fieldNodeId),
            decltype(fieldVesselSize),
            decltype(fieldPeriod),
            decltype(fieldValue)> mSelectParameter;

    sql::WhereStatement mFilterByParameter;

public:
    HarboursVesselsTableQueries(sql::Storage *db)
            : db(db),
              mSelectParameter(*db, HarboursParametersPerVesselSizeTableName,
                               fieldNodeId, fieldVesselSize, fieldPeriod, fieldValue),
              mFilterByParameter(fieldParameter == "param")
    {
    }

    using SetterFunction = std::function<bool(int nodeId,
                                              int vesselSize,
                                              int period,
                                              double value)>;

    void getParameters(std::string parameter,
                       SetterFunction func)
    {
        mSelectParameter.where(mFilterByParameter);
        mSelectParameter.bind(parameter);

        mSelectParameter.execute(func);
    }
};

std::map<types::NodeId, HarboursQueries::FuelPrices> HarboursQueries::getFuelPrices(int period)
{
    HarboursVesselsTableQueries q(p->db);

    std::map<types::NodeId, HarboursQueries::FuelPrices> values;
    q.getParameters("fuelPrice", [&values](int nodeId,
                                           int vesselSize,
                                           int period,
                                           double value) -> bool {
        HarboursQueries::FuelPrices price;
        price.node = types::NodeId(nodeId);
        price.period = period;
        price.vesselSize = vesselSize;
        price.value = value;
        values.insert(std::make_pair(price.node, price));
        return true;
    });

    return values;
}
