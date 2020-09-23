
#include "MetiersLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
    static const char* MetierListTableName = "MetiersSpe";
    static const char* MetierTableName = "MetiersParameters";
    static const char* MetierTableNameScape = "MetiersParametersWithLandscape";
    static const char* MetierTableNameSpSz = "MetiersParametersWithSpeciesAndSzGroup";

    db::Column<db::ColumnTypes::Text> fieldMetiername{ "MetierName" };
    db::Column<db::ColumnTypes::Text> fieldParameter{ "Parameter" };
    db::Column<db::ColumnTypes::Integer> fieldOpt1{ "Opt1" };
    db::Column<db::ColumnTypes::Integer> fieldOpt2{ "Opt2" };
    db::Column<db::ColumnTypes::Integer> fieldPeriod{ "Period" };
    db::Column<db::ColumnTypes::Real> fieldValue{ "Value" };

}


struct MetiersLoader::Impl {
    msqlitecpp::v2::Storage& db;

    /*
        static auto makeSelectStatement (msqlitecpp::v2::Storage &db) {
            return db::makeSelectStatement(db, MetierTableName,
                    fieldOpt1, fieldValue);
        }

        decltype(makeSelectStatement(std::declval<msqlitecpp::v2::Storage&>())) selectStatement;
    */

    db::SelectStatement<
        decltype(fieldOpt1), decltype(fieldValue)
    > selectOpt1ValueFromNameParameter;

    db::SelectStatement<
        decltype(fieldParameter), decltype(fieldOpt1), decltype(fieldOpt2), decltype(fieldValue)>
        selectParamOpt1Opt2ValueFromNamePeriod;

    Impl(msqlitecpp::v2::Storage& thedb);

    std::vector<std::string> getListOfAllMetiers();

    //map<int, double> getInitFuelPrices();

    std::shared_ptr<MetierData> getMetierData(std::string metiername, int period);

};


MetiersLoader::MetiersLoader(msqlitecpp::v2::Storage& db)
    : p(spimpl::make_unique_impl<Impl>(db))
{
}

std::vector<std::string> MetiersLoader::getListOfAllMetiers()
{
    return p->getListOfAllMetiers();
}

//map<int, double> VesselsLoader::getInitFuelPrices()
//{
//    return p->getInitFuelPrices();
//}

shared_ptr<MetiersLoader::MetierData> MetiersLoader::getMetierData(std::string metiername, int period)
{
    return p->getMetierData(metiername, period);
}


/// Implementations

MetiersLoader::Impl::Impl(msqlitecpp::v2::Storage& thedb)
    : db(thedb),
    selectOpt1ValueFromNameParameter(db, MetierTableName, fieldOpt1, fieldValue),
    selectParamOpt1Opt2ValueFromNamePeriod(db, MetierTableName, fieldParameter, fieldOpt1, fieldOpt2, fieldValue)
{
    selectOpt1ValueFromNameParameter.where(fieldMetiername == "name" && fieldParameter == "parameter");
    selectParamOpt1Opt2ValueFromNamePeriod.where(fieldMetiername == "name" && fieldPeriod == "period");
}

std::vector<std::string> MetiersLoader::Impl::getListOfAllMetiers()
{
    std::vector<std::string> metiers;

    auto selectAllMetiers = db::makeSelectStatement(db, MetierListTableName, fieldMetiername);
    selectAllMetiers.execute([&metiers](std::string name) {
        metiers.push_back(name);
        return true;
        });

    return metiers;
}

/*
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
*/

namespace {
    class MetiersLoaderDataDispatcher {
        using func = std::function<void(MetiersLoader::MetierData & data, int, int, double)>;
        using map = std::map<std::string, func>;
        using iterator = map::iterator;
        map dispatcher;

        std::pair<std::string, func> m(std::string p, func f)
        {
            return std::make_pair(p, f);
        }

        std::shared_ptr<MetiersLoader::MetierData> metier;


        static void loadMetierType(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.type = val;
        }

        static void loadMetierBetaPop(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.betas_per_pop.emplace_back(uint16_t(val));
        }
        
        static void loadStockIsAvoided(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.is_avoided_stocks.emplace_back(uint16_t(val));
        }
       
        static void loadpopMLS(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.mls_cat_per_pop.emplace_back(uint16_t(val));
        }

        static void loadDiscardPopRatio(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.discardratio_limits.emplace_back(val); // vector of double
        }

        static void loadTargetStock(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.metier_target_stocks.emplace_back(uint16_t(val)); // vector of double
        }

        static void loadBottomType(MetiersLoader::MetierData& data, int opt, int opt2, double val)
        {
            data.metier_suitable_seabottomtypes.emplace_back(uint16_t(val)); // vector of double
        }

        static void loadDepletionOnHab(MetiersLoader::MetierData& data, int opt1, int opt2, double val)
        {
            data.loss_after_1_passage.insert(std::make_pair(opt1, val)); // should be a multimap!!
        }

        static void loadRevCompleteness(MetiersLoader::MetierData& data, int, int, double val)
        {
            data.percent_revenue_completeness = val; // should be a single double
        }
       
        static void loadSpeedAtFishing(MetiersLoader::MetierData& data, int, int, double val)
        {
            data.fspeed = val; // should be a single double
        }

        static void loadGearWidthA(MetiersLoader::MetierData& data, int, int, double val)
        {
            data.gear_width_a = val; // should be a single double
        }

        static void loadGearWidthB(MetiersLoader::MetierData& data, int, int, double val)
        {
            data.gear_width_b = val; // should be a single double
        }

        static void loadGearWidthModel(MetiersLoader::MetierData& data, int, int, double val)
        {
            data.gear_width_model = val; // should be a single string: TODO!!!
        }

    public:
        explicit MetiersLoaderDataDispatcher(std::shared_ptr<MetiersLoader::MetierData> met) : metier(met)
        {
            if (dispatcher.empty()) {

                dispatcher.insert(m("MetierType", &MetiersLoaderDataDispatcher::loadMetierType));
                dispatcher.insert(m("RevCompleteness", &MetiersLoaderDataDispatcher::loadRevCompleteness));
                //std::vector< vector <double> > selectivity_per_stock; // to find in MetiersParametersWithSpeciesAndSzGroup
                dispatcher.insert(m("MetierBetaPop", &MetiersLoaderDataDispatcher::loadMetierBetaPop));
                dispatcher.insert(m("DiscardPopRatio", &MetiersLoaderDataDispatcher::loadDiscardPopRatio));
                dispatcher.insert(m("StockIsAvoided", &MetiersLoaderDataDispatcher::loadStockIsAvoided));
                dispatcher.insert(m("popMLS", &MetiersLoaderDataDispatcher::loadpopMLS));
                dispatcher.insert(m("SpeedAtFishing", &MetiersLoaderDataDispatcher::loadSpeedAtFishing));
                dispatcher.insert(m("GearWidthA", &MetiersLoaderDataDispatcher::loadGearWidthA));
                dispatcher.insert(m("GearWidthB", &MetiersLoaderDataDispatcher::loadGearWidthB));
                dispatcher.insert(m("GearWidthModel", &MetiersLoaderDataDispatcher::loadGearWidthModel));
                dispatcher.insert(m("DepletionOnHab", &MetiersLoaderDataDispatcher::loadDepletionOnHab)); // to find in MetiersParametersWithLandscape
                dispatcher.insert(m("TargetStock", &MetiersLoaderDataDispatcher::loadTargetStock)); 
                dispatcher.insert(m("BottomType", &MetiersLoaderDataDispatcher::loadBottomType));
       
            }
        }

        void load(std::string parameter, int opt1, int opt2, double val)
        {
            auto function = dispatcher.find(parameter);
            if (function != dispatcher.end()) {
                dispatcher[parameter](*metier, opt1, opt2, val);
            }
            else {
                std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
            }
        }
    };

}

shared_ptr<MetiersLoader::MetierData> MetiersLoader::Impl::getMetierData(std::string metiername, int period)
{
    auto data = std::make_shared<MetiersLoader::MetierData>();

    MetiersLoaderDataDispatcher dispatcher(data);

    selectParamOpt1Opt2ValueFromNamePeriod.bind(metiername, period);
    selectParamOpt1Opt2ValueFromNamePeriod.execute(
        [&data, &dispatcher](std::string param, int opt1, int opt2, double value) {
            dispatcher.load(param, opt1, opt2, value);
            return true;
        });

    return data;
}


