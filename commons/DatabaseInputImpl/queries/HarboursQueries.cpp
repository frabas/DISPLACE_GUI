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
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldMarketCat("marketcat");
static msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Integer> fieldSpecies("species");
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

std::multimap<types::NodeId, HarboursQueries::FuelPrices> HarboursQueries::getFuelPrices(int period)
{
    HarboursVesselsTableQueries q(p->db);

    std::multimap<types::NodeId, HarboursQueries::FuelPrices> values;
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


class HarboursStockTableQueries {
    sql::Storage *db;

    sql::SelectStatement<decltype(fieldNodeId),
            decltype(fieldMarketCat),
            decltype(fieldSpecies),
            decltype(fieldPeriod),
            decltype(fieldValue)> mSelectParameter;

    sql::WhereStatement mFilterByParameter;

public:
    HarboursStockTableQueries(sql::Storage *db)
            : db(db),
              mSelectParameter(*db, HarboursParametersPerVesselSizeTableName,
                               fieldNodeId, fieldMarketCat, fieldSpecies, fieldPeriod, fieldValue),
              mFilterByParameter(fieldParameter == "param")
    {
    }

    using SetterFunction = std::function<bool(int nodeId,
                                              int marketcat,
                                              int species,
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

std::multimap<types::NodeId, HarboursQueries::FishPrices> HarboursQueries::getFishPrices(int period)
{
    HarboursStockTableQueries q(p->db);

    std::multimap<types::NodeId, HarboursQueries::FishPrices> values;
    q.getParameters("fishPrice", [&values](int nodeId,
                                           int marketcat,
                                           int species,
                                           int period,
                                           double value) -> bool {
        HarboursQueries::FishPrices price;
        price.node = types::NodeId(nodeId);
        price.period = period;
        price.marketCat = marketcat;
        price.species = species;
        price.value = value;
        values.insert(std::make_pair(price.node, price));
        return true;
    });

    return values;
}
