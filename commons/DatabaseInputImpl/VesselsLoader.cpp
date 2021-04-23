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
            decltype(fieldParameter), decltype(fieldOpt1), decltype(fieldOpt2), decltype(fieldValue)>
            selectParamOpt1Opt2ValueFromNamePeriod;

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
          selectParamOpt1Opt2ValueFromNamePeriod(db, VesselTableName, fieldParameter, fieldOpt1, fieldOpt2, fieldValue)
{
    selectOpt1ValueFromNameParameter.where(fieldVesselname == "name" && fieldParameter == "parameter");
    selectParamOpt1Opt2ValueFromNamePeriod.where(fieldVesselname == "name" && fieldPeriod == "period");
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

namespace {
class VesselsLoaderDataDispatcher {
    using func = std::function<void(VesselsLoader::VesselData &data, int, int, double)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    map dispatcher;

    std::pair<std::string, func> m(std::string p, func f)
    {
        return std::make_pair(p, f);
    }

    std::shared_ptr<VesselsLoader::VesselData> vessel;

    static void loadFGroundNodelistAndFreq(VesselsLoader::VesselData &data, int opt, int opt2, double val)
    {
        data.fground.emplace_back(uint16_t(opt));
        data.freq_fgrounds.emplace_back(uint16_t(val));
        data.freq_fgrounds_init.emplace_back(uint16_t(val));
        // data.fgroundFreq.insert(std::make_pair(opt, val));
    }

    static void loadHarbourNodeListAndFreq(VesselsLoader::VesselData &data, int opt, int opt2, double val)
    {
        data.harbours.emplace_back(uint16_t(opt));
        data.freq_harbours.emplace_back(uint16_t(val));
    }

    static void loadvesselBeta(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.vessel_betas_per_pop.emplace_back(uint16_t(val)); // should be a vector of double along pops
    }

    static void loadpercentTac(VesselsLoader::VesselData &data, int opt, int opt2, double val)
    {
        data.percent_tac_per_pop.emplace_back(uint16_t(val)); // should be a vector of double along pops
    }

    static void loadMetierlistOnNodeAndFreq(VesselsLoader::VesselData &data, int opt1, int opt2, double val)
    {
        data.possible_metiers.insert(std::make_pair(types::NodeId(opt2), opt1)); // should be a multimap!!
        data.freq_possible_metiers.insert(std::make_pair(types::NodeId(opt2), val)); // should be a multimap!!
    }

    static void loadGShapeLPUE(VesselsLoader::VesselData &data, int opt1, int opt2, double val)
    {
        data.gshape_cpue_per_stk_on_nodes.insert(std::make_pair(types::NodeId(opt1), val)); // should be a multimap!!
    }

    static void loadGScaleLPUE(VesselsLoader::VesselData &data, int opt1, int opt2, double val)
    {
        data.gscale_cpue_per_stk_on_nodes.insert(std::make_pair(types::NodeId(opt1), val)); // should be a multimap!!
    }


    static void loadIsVesActive(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.vid_is_active = uint16_t(val); // should be a single int
    }

    static void loadIsVesRefFleet(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.vid_is_part_of_ref_fleet = uint16_t(val); // should be a single int
    }

    static void loadVesSpeed(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.speed = val; // should be a single double
    }

    static void loadFuelLitrePerH(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.fuelcons = val; // should be a single double
    }

    static void loadVesLength(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.length = val; // should be a single double
    }

    static void loadVesKW(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.KW = val; // should be a single double
    }

    static void loadVesStorageKg(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.carrycapacity = val; // should be a single double
    }

    static void loadVesFuelTankLitre(VesselsLoader::VesselData &data, int, int opt2, double val)
    {
        data.tankcapacity = val; // should be a single double
    }

    static void loadNbPingsPerTrip(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.nbfpingspertrip = val; // should be a single double
    }

    static void loadRestTimeParam1(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.resttime_par1 = val; // should be a single double
    }

    static void loadRestTimeParam2(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.resttime_par2 = val; // should be a single double
    }

    static void loadTripDuration(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.av_trip_duration = val; // should be a single double
    }

    static void loadFuelMultiSteamg(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.mult_fuelcons_when_steaming = val; // should be a single double
    }

    static void loadFuelMultiFishg(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.mult_fuelcons_when_fishing = val; // should be a single double
    }

    static void loadFuelMultiReturg(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.mult_fuelcons_when_returning = val; // should be a single double
    }

    static void loadFuelMultiInactiv(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.mult_fuelcons_when_inactive = val; // should be a single double
    }

    static void loadFirmID(VesselsLoader::VesselData &data, int, int, double val)
    {
        data._firm_id = uint16_t(val); // should be a single int
    }

    static void loadNbCrew(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.this_vessel_nb_crew = val; // should be a single double
    }

    static void loadAnnlOthIncome(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.annual_other_income = val; // should be a single double
    }

    static void loadLandCostsPercent(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.landing_costs_percent = val; // should be a single double
    }

    static void loadCrewsharePercent(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.crewshare_and_unpaid_labour_costs_percent = val; // should be a single double
    }

    static void loadVarOthCostsPerEff(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.other_variable_costs_per_unit_effort = val; // should be a single double
    }

    static void loadAnnlInsurCostsPerCrew(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.annual_insurance_costs_per_crew = val; // should be a single double
    }

    static void loadLabourOpportyCosts(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.standard_labour_hour_opportunity_costs = val; // should be a single double
    }

    static void loadAnnlFTEhours(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.standard_annual_full_time_employement_hours = val; // should be a single double
    }

    static void loadAnnlOthFixedCosts(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.other_annual_fixed_costs = val; // should be a single double
    }

    static void loadVesValue(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.vessel_value = val; // should be a single double
    }

    static void loadAnnlDeprecRate(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.annual_depreciation_rate = val; // should be a single double
    }

    static void loadOpportyInterestRate(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.opportunity_interest_rate = val; // should be a single double
    }

    static void loadAnnlDiscountRate(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.annual_discount_rate = val; // should be a single double
    }

    static void loadWorkHoursStart(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.calendar.workStartHour = static_cast<int>(val);
    }

    static void loadWorkHoursEnd(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.calendar.workEndHour = static_cast<int>(val);
    }

    static void loadWeekEndStartDay(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.calendar.weekEndStartDay = static_cast<int>(val);
    }

    static void loadWeekEndEndDay(VesselsLoader::VesselData &data, int, int, double val)
    {
        data.calendar.weekEndEndDay = static_cast<int>(val);
    }

public:
    explicit VesselsLoaderDataDispatcher(std::shared_ptr<VesselsLoader::VesselData> v) : vessel(v)
    {
        if (dispatcher.empty()) {

            dispatcher.insert(m("FGroundNodelistAndFreq", &VesselsLoaderDataDispatcher::loadFGroundNodelistAndFreq));
            dispatcher.insert(m("HarbourNodeListAndFreq", &VesselsLoaderDataDispatcher::loadHarbourNodeListAndFreq));
            dispatcher.insert(m("vesselBeta", &VesselsLoaderDataDispatcher::loadvesselBeta));
            dispatcher.insert(m("percentTac", &VesselsLoaderDataDispatcher::loadpercentTac));
            dispatcher.insert(m("MetierlistOnNodeAndFreq", &VesselsLoaderDataDispatcher::loadMetierlistOnNodeAndFreq));
            dispatcher.insert(m("GShapeLPUE", &VesselsLoaderDataDispatcher::loadGShapeLPUE));
            dispatcher.insert(m("GScaleLPUE", &VesselsLoaderDataDispatcher::loadGScaleLPUE));
            dispatcher.insert(m("IsVesActive", &VesselsLoaderDataDispatcher::loadIsVesActive));
            dispatcher.insert(m("IsVesRefFleet", &VesselsLoaderDataDispatcher::loadIsVesRefFleet));
            dispatcher.insert(m("VesSpeed", &VesselsLoaderDataDispatcher::loadVesSpeed));
            dispatcher.insert(m("FuelLitrePerH", &VesselsLoaderDataDispatcher::loadFuelLitrePerH));
            dispatcher.insert(m("VesLength", &VesselsLoaderDataDispatcher::loadVesLength));
            dispatcher.insert(m("VesKW", &VesselsLoaderDataDispatcher::loadVesKW));
            dispatcher.insert(m("VesStorageKg", &VesselsLoaderDataDispatcher::loadVesStorageKg));
            dispatcher.insert(m("VesFuelTankLitre", &VesselsLoaderDataDispatcher::loadVesFuelTankLitre));
            dispatcher.insert(m("NbPingsPerTrip", &VesselsLoaderDataDispatcher::loadNbPingsPerTrip));
            dispatcher.insert(m("RestTimeParam1", &VesselsLoaderDataDispatcher::loadRestTimeParam1));
            dispatcher.insert(m("RestTimeParam2", &VesselsLoaderDataDispatcher::loadRestTimeParam2));
            dispatcher.insert(m("TripDuration", &VesselsLoaderDataDispatcher::loadTripDuration));
            dispatcher.insert(m("FuelMultiSteamg", &VesselsLoaderDataDispatcher::loadFuelMultiSteamg));
            dispatcher.insert(m("FuelMultiFishg", &VesselsLoaderDataDispatcher::loadFuelMultiFishg));
            dispatcher.insert(m("FuelMultiReturg", &VesselsLoaderDataDispatcher::loadFuelMultiReturg));
            dispatcher.insert(m("FuelMultiInactiv", &VesselsLoaderDataDispatcher::loadFuelMultiInactiv));
            dispatcher.insert(m("FirmID", &VesselsLoaderDataDispatcher::loadFirmID));
            dispatcher.insert(m("WorkHoursStart", &VesselsLoaderDataDispatcher::loadWorkHoursStart));
            dispatcher.insert(m("WorkHoursEnd", &VesselsLoaderDataDispatcher::loadWorkHoursEnd));
            dispatcher.insert(m("WeekEndStartDay", &VesselsLoaderDataDispatcher::loadWeekEndStartDay));

            // TODO remove this, for legacy purpose.
            dispatcher.insert(m("weekEndStartDay", &VesselsLoaderDataDispatcher::loadWeekEndStartDay));

            dispatcher.insert(m("WeekEndEndDay", &VesselsLoaderDataDispatcher::loadWeekEndEndDay));
            dispatcher.insert(m("NbCrew", &VesselsLoaderDataDispatcher::loadNbCrew));
            dispatcher.insert(m("AnnlOthIncome", &VesselsLoaderDataDispatcher::loadAnnlOthIncome));
            dispatcher.insert(m("LandCostsPercent", &VesselsLoaderDataDispatcher::loadLandCostsPercent));
            dispatcher.insert(m("CrewsharePercent", &VesselsLoaderDataDispatcher::loadCrewsharePercent));
            dispatcher.insert(m("VarOthCostsPerEff", &VesselsLoaderDataDispatcher::loadVarOthCostsPerEff));
            dispatcher.insert(m("AnnlInsurCostsPerCrew", &VesselsLoaderDataDispatcher::loadAnnlInsurCostsPerCrew));
            dispatcher.insert(m("LabourOpportyCosts", &VesselsLoaderDataDispatcher::loadLabourOpportyCosts));
            dispatcher.insert(m("AnnlFTEhours", &VesselsLoaderDataDispatcher::loadAnnlFTEhours));
            dispatcher.insert(m("AnnlOthFixedCosts", &VesselsLoaderDataDispatcher::loadAnnlOthFixedCosts));
            dispatcher.insert(m("VesValue", &VesselsLoaderDataDispatcher::loadVesValue));
            dispatcher.insert(m("AnnlDeprecRate", &VesselsLoaderDataDispatcher::loadAnnlDeprecRate));
            dispatcher.insert(m("OpportyInterestRate", &VesselsLoaderDataDispatcher::loadOpportyInterestRate));
            dispatcher.insert(m("AnnlDiscountRate", &VesselsLoaderDataDispatcher::loadAnnlDiscountRate));

         // TODO check why some vars in VesselsParameters db table not used: FixedLPUE FuelPrice 
        
        }
    }

    void load(std::string parameter, int opt1, int opt2, double val)
    {
        auto function = dispatcher.find(parameter);
        if (function != dispatcher.end()) {
            dispatcher[parameter](*vessel, opt1, opt2, val);
        } else {
            std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
        }
    }
};

}

shared_ptr<VesselsLoader::VesselData> VesselsLoader::Impl::getVesselData(std::string vesselname, int period)
{
    auto data = std::make_shared<VesselsLoader::VesselData>();

    VesselsLoaderDataDispatcher dispatcher(data);

    selectParamOpt1Opt2ValueFromNamePeriod.bind(vesselname, period);
    selectParamOpt1Opt2ValueFromNamePeriod.execute(
            [&data, &dispatcher](std::string param, int opt1, int opt2, double value) {
                dispatcher.load(param, opt1, opt2, value);
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
