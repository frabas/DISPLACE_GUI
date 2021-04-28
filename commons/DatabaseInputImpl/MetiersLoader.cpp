
#include "MetiersLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
static const char *MetierListTableName = "MetiersSpe";
static const char *MetierTableName = "MetiersParameters";
static const char *MetierTableNameScape = "MetiersParametersWithLandscape";
static const char *MetierTableNameSpSz = "MetiersParametersWithSpeciesAndSzGroup";

db::Column<db::ColumnTypes::Text> fieldMetiername{"MetierName"};
db::Column<db::ColumnTypes::Text> fieldParameter{"Parameter"};
db::Column<db::ColumnTypes::Integer> fieldOpt1{"Opt1"};
db::Column<db::ColumnTypes::Integer> fieldOpt2{"Opt2"};
db::Column<db::ColumnTypes::Integer> fieldSpecies{"species"};
db::Column<db::ColumnTypes::Integer> fieldSzGroup{"szgroup"};
db::Column<db::ColumnTypes::Integer> fieldFuncGroup{"funcgroup"};
db::Column<db::ColumnTypes::Integer> fieldLandscape{"landscape"};
db::Column<db::ColumnTypes::Integer> fieldPeriod{"Period"};
db::Column<db::ColumnTypes::Real> fieldValue{"Value"};

}

template<typename V>
V& at(std::vector<V>& v, int index)
{
    while (v.size() <= index) {
        v.emplace_back(V{});
    }
    return v.at(index);
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

    db::SelectStatement<
            decltype(fieldParameter), decltype(fieldSpecies), decltype(fieldSzGroup), decltype(fieldValue)>
            selectParamSpeciesSzGroup;

    db::SelectStatement<
        decltype(fieldParameter), decltype(fieldFuncGroup), decltype(fieldLandscape), decltype(fieldValue)>
        selectParamFuncGroupLandscape;
    

    Impl(msqlitecpp::v2::Storage &thedb);

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
      selectParamOpt1Opt2ValueFromNamePeriod(db, MetierTableName, fieldParameter, fieldOpt1, fieldOpt2, fieldValue),
      selectParamSpeciesSzGroup(db, MetierTableNameSpSz, fieldParameter, fieldSpecies, fieldSzGroup, fieldValue),
      selectParamFuncGroupLandscape(db, MetierTableNameScape, fieldParameter, fieldFuncGroup, fieldLandscape, fieldValue)
{
    selectOpt1ValueFromNameParameter.where(fieldMetiername == "MetierName" && fieldParameter == "parameter");
    selectParamOpt1Opt2ValueFromNamePeriod.where(fieldMetiername == "MetierName" && fieldPeriod == "period");
    selectParamSpeciesSzGroup.where(
            fieldMetiername == "MetierName" &&
            msqlitecpp::v2::WhereStatement((fieldPeriod == "period"), "OR",
                                           msqlitecpp::v2::WhereStatement(fieldPeriod.name(), "is", "null")));
    selectParamFuncGroupLandscape.where(
            fieldMetiername == "MetierName" &&
            msqlitecpp::v2::WhereStatement((fieldPeriod == "period"), "OR",
                msqlitecpp::v2::WhereStatement(fieldPeriod.name(), "is", "null")));
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
            } else {
                std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
            }
        }
    };


class MetiersSpeciesSzGroupDispatcher {
    using func = std::function<void(MetiersLoader::MetierData &data, int, int, double)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;
    std::shared_ptr<MetiersLoader::MetierData> metier;

    static void loadSelectivityPerStock(MetiersLoader::MetierData &data, int species, int szgroup, double val)
    {
        //std::cout << "SelOgive (" << species << "," << szgroup << ") =" << val << "\n";
        at(at(data.selectivity_per_stock, species), szgroup) = val;
    }

    static void loadSelectivityOthPerStock(MetiersLoader::MetierData& data, int species, int szgroup, double val)
    {
        at(at(data.selectivity_per_stock_ogives_for_oth_land, species), szgroup) = val;   
    }

public:
    explicit MetiersSpeciesSzGroupDispatcher(std::shared_ptr<MetiersLoader::MetierData> met) : metier(met)
    {
        if (dispatcher.empty()) {
            dispatcher.insert(
                    std::make_pair("SelOgive", &MetiersSpeciesSzGroupDispatcher::loadSelectivityPerStock));
            dispatcher.insert(
                    std::make_pair("SelOgiveOth", &MetiersSpeciesSzGroupDispatcher::loadSelectivityOthPerStock)); // TODO: add to metier creator()
        }
    }

    void load(std::string parameter, int species, int szgroup, double val)
    {
        auto function = dispatcher.find(parameter);
        if (function != dispatcher.end()) {
            dispatcher[parameter](*metier, species, szgroup, val);
        } else {
            std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
        }
    }
};

MetiersSpeciesSzGroupDispatcher::map MetiersSpeciesSzGroupDispatcher::dispatcher;





//for retrieving loss_after_1_passage:
class MetiersFuncGroupLandscapeDispatcher {
    using func = std::function<void(MetiersLoader::MetierData & data, int, int, double)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;
    std::shared_ptr<MetiersLoader::MetierData> metier;

    static void loadDepletionOnHab(MetiersLoader::MetierData& data, int landscape, int funcgroup, double val)
    {
        data.loss_after_1_passage.insert(std::make_pair(landscape, val));
    }

 

public:
    explicit MetiersFuncGroupLandscapeDispatcher(std::shared_ptr<MetiersLoader::MetierData> met) : metier(met)
    {
        if (dispatcher.empty()) {
            dispatcher.insert(
                std::make_pair("DepletionOnHab", &MetiersFuncGroupLandscapeDispatcher::loadDepletionOnHab));
        }
    }

    void load(std::string parameter, int funcgroup, int landscape, double val)
    {
        auto function = dispatcher.find(parameter);
        if (function != dispatcher.end()) {
            dispatcher[parameter](*metier, funcgroup, landscape, val);
        }
        else {
            std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
        }
    }
};

MetiersFuncGroupLandscapeDispatcher::map MetiersFuncGroupLandscapeDispatcher::dispatcher;

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

    MetiersSpeciesSzGroupDispatcher szGroupDispatcher(data);
    selectParamSpeciesSzGroup.bind(metiername, period);

    std::cout << "SQL: " << selectParamSpeciesSzGroup.toString() << "; %1="
              << metiername << ", %2=" << period << "\n";
    selectParamSpeciesSzGroup.execute(
            [&data, &szGroupDispatcher](std::string param, int species, int group, double value) {
                szGroupDispatcher.load(param, species, group, value);
                return true;
            });

    MetiersFuncGroupLandscapeDispatcher FuncGroupLandscapeDispatcher(data);
    selectParamFuncGroupLandscape.bind(metiername, period);
    std::cout << "SQL: " << selectParamFuncGroupLandscape.toString() << "; %1="
        << metiername << ", %2=" << period << "\n";
    selectParamFuncGroupLandscape.execute(
            [&data, &FuncGroupLandscapeDispatcher](std::string param, int landscape, int funcgroup, double value) {
                FuncGroupLandscapeDispatcher.load(param, landscape, funcgroup, value);
                return true;
            });


    return data;
}


