//
// Created by fuga on 02/11/2019.
//

#include "VesselsLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
static const char *VesselListTableName = "VesselsSpe";
static const char *VesselTableName = "VesselsParameters";

db::Column<db::ColumnTypes::Text> fieldVesselname{"VesselName"};
db::Column<db::ColumnTypes::Text> fieldParameter{"Parameter"};
db::Column<db::ColumnTypes::Integer> fieldOpt1{"Opt1"};
db::Column<db::ColumnTypes::Integer> fieldOpt2{"Opt2"};
db::Column<db::ColumnTypes::Integer> fieldPeriod{"Period"};
db::Column<db::ColumnTypes::Real> fieldValue{"Value"};

}


struct VesselsLoader::Impl {
    msqlitecpp::v2::Storage &db;

/*
    static auto makeSelectStatement (msqlitecpp::v2::Storage &db) {
        return db::makeSelectStatement(db, VesselTableName,
                fieldOpt1, fieldValue);
    }

    decltype(makeSelectStatement(std::declval<msqlitecpp::v2::Storage&>())) selectStatement;
*/

    db::SelectStatement<
            decltype(fieldOpt1), decltype(fieldValue)
    > selectOpt1ValueFromNameParameter;

    db::SelectStatement<
            decltype(fieldParameter), decltype(fieldOpt1), decltype(fieldValue)>
            selectParamOpt1ValueFromNamePeriod;

    Impl(msqlitecpp::v2::Storage &thedb);

    std::vector<std::string> getListOfAllVessels();

    map<int, double> getInitFuelPrices();

    std::shared_ptr<VesselData> getVesselData(std::string vesselname, int period);

    std::vector<double> getEconomicFeature(std::string);
};


VesselsLoader::VesselsLoader(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{
}

std::vector<std::string> VesselsLoader::getListOfAllVessels()
{
    return p->getListOfAllVessels();
}

map<int, double> VesselsLoader::getInitFuelPrices()
{
    return p->getInitFuelPrices();
}

shared_ptr<VesselsLoader::VesselData> VesselsLoader::getVesselData(std::string vesselname, int period)
{
    return p->getVesselData(vesselname, period);
}

std::vector<double> VesselsLoader::getEconomicFeature(std::string vesselname)
{
    return p->getEconomicFeature(vesselname);
}

/// Implementations

VesselsLoader::Impl::Impl(msqlitecpp::v2::Storage &thedb)
        : db(thedb),
          selectOpt1ValueFromNameParameter(db, VesselTableName, fieldOpt1, fieldValue),
          selectParamOpt1ValueFromNamePeriod(db, VesselTableName, fieldParameter, fieldOpt1, fieldValue)
{
    selectOpt1ValueFromNameParameter.where(fieldVesselname == "name" && fieldParameter == "parameter");
    selectParamOpt1ValueFromNamePeriod.where(fieldVesselname == "name" && fieldPeriod == "period");
}

std::vector<std::string> VesselsLoader::Impl::getListOfAllVessels()
{
    std::vector<std::string> vessels;

    auto selectAllVessels = db::makeSelectStatement(db, VesselListTableName, fieldVesselname);
    selectAllVessels.execute([&vessels](std::string name) {
        vessels.push_back(name);
        return true;
    });

    return vessels;
}

map<int, double> VesselsLoader::Impl::getInitFuelPrices()
{
    map<int, double> prices;

    auto selectStatement = db::makeSelectStatement(db, VesselTableName, fieldOpt1, fieldValue);
    selectStatement.where(fieldParameter == "param");
    selectStatement.bind("FuelPrice");
    selectStatement.execute([&prices](int opt, double value) {
        prices.insert(std::make_pair(opt, value));
        return true;
    });
    return prices;
}

shared_ptr<VesselsLoader::VesselData> VesselsLoader::Impl::getVesselData(std::string vesselname, int period)
{
    auto data = std::make_shared<VesselsLoader::VesselData>();

    selectParamOpt1ValueFromNamePeriod.bind(vesselname, period);
    selectParamOpt1ValueFromNamePeriod.execute([&data](std::string param, int opt1, double value) {
        if (param == "fgroundFreq") {
            data->fground.push_back(opt1);
            data->fgroundFreq.insert(std::make_pair(opt1, value));
        }
        return true;
    });

    return data;
}

std::vector<double> VesselsLoader::Impl::getEconomicFeature(std::string name)
{
    std::vector<double> features;
    selectOpt1ValueFromNameParameter.bind(name, "economic_feature");

    selectOpt1ValueFromNameParameter.execute([&features](int opt1, double value) {
        if (features.size() < opt1 + 1) {
            features.resize(opt1 + 1);
        }
        features[opt1] = value;
        return true;
    });

    return features;
}
