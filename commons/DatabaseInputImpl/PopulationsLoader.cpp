
#include "PopulationsLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
    static const char* PopulationListTableName = "Populations";
    static const char* PopulationTableName = "PopulationParameters";
    static const char* PopulationTableNameTrM = "PopulationTransitionMatrix";
    static const char* PopulationTableNameSzAg = "PopulationParametersWithSizeGroupAndAge";

    db::Column<db::ColumnTypes::Integer> fieldPopId{ "id" };
    db::Column<db::ColumnTypes::Integer> fieldSpecies{ "pop_id" };
    db::Column<db::ColumnTypes::Text> fieldPopName{ "name" };
    db::Column<db::ColumnTypes::Integer> fieldBioSce{ "biosce" };
    db::Column<db::ColumnTypes::Text> fieldParameter{ "parameter" };
    db::Column<db::ColumnTypes::Integer> fieldSzGroup{ "size_group" };
    db::Column<db::ColumnTypes::Integer> fieldSzGroup1{ "sizegroup1" };
    db::Column<db::ColumnTypes::Integer> fieldSzGroup2{ "sizegroup2" };
    db::Column<db::ColumnTypes::Integer> fieldAge{ "age" };
    db::Column<db::ColumnTypes::Integer> fieldAge{ "node" };
    db::Column<db::ColumnTypes::Integer> fieldFuncGroup{ "landscape" };
    db::Column<db::ColumnTypes::Text> fieldCountry{ "country" };
    db::Column<db::ColumnTypes::Integer> fieldPeriod{ "period" };
    db::Column<db::ColumnTypes::Real> fieldValue{ "value" };

}


struct PopulationsLoader::Impl {
    msqlitecpp::v2::Storage& db;

    db::SelectStatement<
        decltype(fieldPopId), decltype(fieldValue)
    > selectPopValueFromNameParameter;

    db::SelectStatement<
        decltype(fieldCountry), decltype(fieldValue)
    > selectCountryValueFromNameParameter;

    db::SelectStatement<
        decltype(fieldParameter), decltype(fieldBioSce), decltype(fieldPeriod), decltype(fieldValue)>
        selectParamBioscePeriodValueFromNamePeriod;

    db::SelectStatement<
        decltype(fieldParameter), decltype(fieldSpecies), decltype(fieldSzGroup1), decltype(fieldSzGroup2), decltype(fieldValue)>
        selectParamSpeciesTrM;

    db::SelectStatement<
        decltype(fieldParameter), decltype(fieldSpecies), decltype(fieldSzGroup), decltype(fieldAge), decltype(fieldValue)>
        selectParamSpeciesSzAg;
    
    Impl(msqlitecpp::v2::Storage& thedb);

    std::vector<std::string> getListOfAllPopulations();

    std::shared_ptr<PopulationData> getPopulationData(std::string popname, int period);

};


PopulationsLoader::PopulationsLoader(msqlitecpp::v2::Storage& db)
    : p(spimpl::make_unique_impl<Impl>(db))
{
}

std::vector<std::string> PopulationsLoader::getListOfAllPopulations()
{
    return p->getListOfAllPopulations();
}

shared_ptr<PopulationsLoader::PopulationData> PopulationsLoader::getPopulationData(std::string popname, int period)
{
    return p->getPopulationData(popname, period);
}


/// Implementations

PopulationsLoader::Impl::Impl(msqlitecpp::v2::Storage& thedb)
    : db(thedb),
    selectPopValueFromNameParameter(db, PopulationTableName, fieldSpecies, fieldValue),
    selectCountryValueFromNameParameter(db, PopulationTableName, fieldCountry, fieldValue),
    selectParamBioscePeriodValueFromNamePeriod(db, PopulationTableName, fieldParameter, fieldBioSce, fieldPeriod, fieldValue),
    selectParamSpeciesTrM(db, PopulationTableNameTrM, fieldParameter, fieldSpecies, fieldSzGroup1, fieldSzGroup2, fieldValue),
    selectParamSpeciesSzAg(db, PopulationTableNameSzAg, fieldParameter, fieldSpecies, fieldSzGroup, fieldAge, fieldValue)
{
    selectPopValueFromNameParameter.where(fieldSpecies == "pop_id");
    selectCountryValueFromNameParameter.where(fieldSpecies == "pop_id" && fieldCountry == "country");
    selectParamBioscePeriodValueFromNamePeriod.where(
        fieldParameter == "parameter" && fieldSpecies == "pop_id" && fieldSzGroup == "szgroup");
    selectParamSpeciesTrM.where(
        fieldParameter == "parameter" && fieldSpecies == "pop_id" && fieldSzGroup1 == "sizegroup1" && fieldSzGroup2 == "sizegroup2");
    selectParamSpeciesSzAg.where(
        fieldParameter == "parameter" && fieldSpecies == "pop_id" && fieldSzGroup == "size_group" && fieldAge == "age");
}

std::vector<std::string> PopulationsLoader::Impl::getListOfAllPopulations()
{
    std::vector<std::string> populations;

    auto selectAllPopulations = db::makeSelectStatement(db, PopulationListTableName, fieldSpecies);
    selectAllPopulations.execute([&populations](std::string name) {
        populations.push_back(name);
        return true;
        });

    return populations;
}



namespace {
    class PopulationsLoaderDataDispatcher {
        using func = std::function<void(PopulationsLoader::PopulationData & data, int, int, double)>;
        using map = std::map<std::string, func>;
        using iterator = map::iterator;
        map dispatcher;

        std::pair<std::string, func> m(std::string p, func f)
        {
            return std::make_pair(p, f);
        }

        std::shared_ptr<PopulationsLoader::PopulationData> population;


       // static void loadPopulationType(PopulationsLoader::PopulationData& data, int opt, int opt2, double val)
       // {
       //     data.type = val;
       // }

       // static void loadPopulationBetaPop(PopulationsLoader::PopulationData& data, int opt, int opt2, double val)
       // {
       //     data.betas_per_pop.emplace_back(uint16_t(val));
       // }

      
    public:
        explicit PopulationsLoaderDataDispatcher(std::shared_ptr<PopulationsLoader::PopulationData> pop) : population(pop)
        {
            if (dispatcher.empty()) {
                
                //dispatcher.insert(m("MetierType", &PopulationsLoaderDataDispatcher::loadPopulationType));
                //dispatcher.insert(m("RevCompleteness", &PopulationsLoaderDataDispatcher::loadPopulationBetaPop));
                
                // TODO PopulationParameters db table
                //int a_name;
                //string a_pop_name;
                //std::vector<double> param_sr;
                //std::map<string, double> relative_stability_key;
                //std::vector<double> fbar_ages_min_max;
                //std::vector<double> init_tac;
                //double tac_percent_simulated;
        
            
            }
        }

        void load(std::string parameter, int opt1, int opt2, double val)
        {
            auto function = dispatcher.find(parameter);
            if (function != dispatcher.end()) {
                dispatcher[parameter](*population, opt1, opt2, val);
            }
            else {
                std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
            }
        }
    };





    classPopulationsSpeciesSzGroupAgeDispatcher {
        using func = std::function<void(PopulationsLoader::PopulationData & data, int, int, int, double)>;
        using map = std::map<std::string, func>;
        using iterator = map::iterator;
        static map dispatcher;
        std::shared_ptr<PopulationsLoader::PopulationData> population;

      //  static void loadSelectivityPerStock(PopulationsLoader::PopulationData& data, int species, int szgroup, int age, double val)
      //  {
      //      data.selectivity_per_stock.at(species).at(szgroup) = val;
      //  }

    
    public:
        explicit PopulationsSpeciesSzGroupAgeDispatcher(std::shared_ptr<PopulationsLoader::PopulationData> pop) : population(pop)
        {
            if (dispatcher.empty()) {
                dispatcher.insert(
                 //   std::make_pair("SelOgive", &MetiersSpeciesSzGroupDispatcher::loadSelectivityPerStock));
    
                // TODO PopulationParametersWithSizeGroupAndAge db table
        /*double avai0_beta;
        double avai2_beta;
        double avai3_beta;
        double avai5_beta;
        double avai7_beta;
        std::vector<int> selected_szgroups;
        std::vector<double> init_tot_N_at_szgroup;
        std::vector<double> init_prop_migrants_in_tot_N_at_szgroup;
        std::vector<double> init_fecundity_at_szgroup;
        std::vector<double> init_weight_at_szgroup;
        std::vector<int> init_comcat_at_szgroup;
        std::vector<double> init_maturity_at_szgroup;
        std::vector<double> init_M_at_szgroup;
        std::vector<double> init_proprecru_at_szgroup;
        std::multimap<types::NodeId, double> full_spatial_availability;
        std::multimap<types::NodeId, double> field_of_coeff_diffusion_this_pop;
        std::map<types::NodeId, double> oth_land;
        std::multimap<int, double> overall_migration_fluxes;
        std::map<string, double> relative_stability_key;
        std::vector<std::vector<double> > percent_szgroup_per_age_matrix;
        std::vector<std::vector<double> > percent_age_per_szgroup_matrix;
        std::vector<Node*> nodes;
        double hyperstability_param;
        double a_calib_cpue_multiplier;
        double a_calib_weight_at_szgroup;
        */
            
            }
        }

        void load(std::string parameter, int species, int szgroup, int age, double val)
        {
            auto function = dispatcher.find(parameter);
            if (function != dispatcher.end()) {
                dispatcher[parameter](*population, species, szgroup, age, val);
            }
            else {
                std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
            }
        }
    };

    PopulationsSpeciesSzGroupAgeDispatcher::map PopulationsSpeciesSzGroupAgeDispatcher::dispatcher;






    classPopulationsSpeciesSzGroupSzGroupDispatcher{
        using func = std::function<void(PopulationsLoader::PopulationData & data, int, int, int, double)>;
        using map = std::map<std::string, func>;
        using iterator = map::iterator;
        static map dispatcher;
        std::shared_ptr<PopulationsLoader::PopulationData> population;

        //  static void loadSelectivityPerStock(PopulationsLoader::PopulationData& data, int species, int szgroup, int age, double val)
        //  {
        //      data.selectivity_per_stock.at(species).at(szgroup) = val;
        //  }


      public:
          explicit PopulationsSpeciesSzGroupSzGroupDispatcher(std::shared_ptr<PopulationsLoader::PopulationData> pop) : population(pop)
          {
              if (dispatcher.empty()) {
                  dispatcher.insert(
                    //  std::make_pair("SelOgive", &MetiersSpeciesSzGroupDispatcher::loadSelectivityPerStock));

                  // TODO PopulationParametersWithSizeGroupAndAge db table
                  //std::vector<std::vector<double> > growth_transition_matrix;
                  
              }
          }

          void load(std::string parameter, int species, int szgroup, int age, double val)
          {
              auto function = dispatcher.find(parameter);
              if (function != dispatcher.end()) {
                  dispatcher[parameter](*population, species, szgroup1, szgroup2, val);
              }
              else {
                  std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
              }
          }
    };

    PopulationsSpeciesSzGroupSzGroupDispatcher::map PopulationsSpeciesSzGroupSzGroupDispatcher::dispatcher;



