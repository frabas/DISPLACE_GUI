
#include "PopulationsLoader.h"
#include "Population.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
static const char *PopulationListTableName = "Populations";
static const char *PopulationTableName = "PopulationParameters";
static const char *PopulationTableNameTrM = "PopulationTransitionMatrix";
static const char *PopulationTableNameSzAg = "PopulationParametersWithSizeGroupAndAge";

db::Column<db::ColumnTypes::Integer> fieldPopId{"id"};
db::Column<db::ColumnTypes::Integer> fieldSpecies{"pop_id"};
db::Column<db::ColumnTypes::Text> fieldPopName{"name"};
db::Column<db::ColumnTypes::Integer> fieldBioSce{"biosce"};
db::Column<db::ColumnTypes::Text> fieldParameter{"parameter"};
db::Column<db::ColumnTypes::Integer> fieldSzGroup{"size_group"};
db::Column<db::ColumnTypes::Integer> fieldSzGroup1{"sizegroup1"};
db::Column<db::ColumnTypes::Integer> fieldSzGroup2{"sizegroup2"};
db::Column<db::ColumnTypes::Integer> fieldAge{"age"};
db::Column<db::ColumnTypes::Integer> fieldNode{"node"};
db::Column<db::ColumnTypes::Integer> fieldLandscape{"landscape"};
db::Column<db::ColumnTypes::Text> fieldCountry{"country"};
db::Column<db::ColumnTypes::Integer> fieldPeriod{"period"};
db::Column<db::ColumnTypes::Real> fieldValue{"value"};
}


class PopulationAllQuery {
    msqlitecpp::v2::Storage &db;
public:
    explicit PopulationAllQuery(msqlitecpp::v2::Storage &_db) : db(_db)
    {}

    std::vector<std::pair<int, string>> execute()
    {
        std::vector<std::pair<int, string>> populations;

        auto selectAllPopulations = db::makeSelectStatement(db, PopulationListTableName, fieldPopId, fieldPopName);
        selectAllPopulations.execute([&populations](int id, std::string name) {
            populations.emplace_back(std::make_pair(id, std::move(name)));
            return true;
        });

        return populations;
    }
};

class PopulationParametersQuery {
    msqlitecpp::v2::Storage &db;
    using func = std::function<void(PopulationsLoader::PopulationData &population, int, int, double)>;
    using map = std::map<std::string, func>;
    using iterator = map::iterator;
    static map dispatcher;

    db::SelectStatement<
            decltype(fieldParameter), decltype(fieldCountry), decltype(fieldPeriod), decltype(fieldLandscape), decltype(fieldValue)
    > selectQuery;

    void dispatch(PopulationsLoader::PopulationData &population, std::string parameter, std::string country, int period,
                  int landscape,
                  double value)
    {
        auto function = dispatcher.find(parameter);
        if (function != dispatcher.end()) {
            (function->second)(population, period, landscape, value);
        } else {
            std::cout << "** Warning, Can't load Parameter " << parameter << " not implemented.\n";
        }
    }

public:
    explicit PopulationParametersQuery(msqlitecpp::v2::Storage &_db)
            : db(_db),
              selectQuery(db, PopulationTableName, fieldParameter, fieldCountry, fieldPeriod, fieldLandscape,
                          fieldValue)
    {
        if (dispatcher.empty()) {
            // TODO fill here with the proper insert
        }
    }

    void execute(PopulationsLoader::PopulationData &population)
    {
        selectQuery.execute([this, &population](std::string parameter, std::string country, int period, int landscape,
                                                double value) {
            dispatch(population, parameter, country, period, landscape, value);
            return true;
        });
    }

    PopulationsLoader::PopulationData executeForNamePeriod(std::string popname, int period)
    {
        PopulationsLoader::PopulationData p;

        selectQuery.where(fieldPopName == "popname" && fieldPeriod == "period");
        selectQuery.bind(popname, period);
        selectQuery.execute([this, &p](std::string parameter, std::string country, int period, int landscape,
                                       double value) {
            dispatch(p, parameter, country, period, landscape, value);
            return true;
        });

        return p;
    }
};

PopulationParametersQuery::map PopulationParametersQuery::dispatcher;

struct PopulationsLoader::Impl {
    msqlitecpp::v2::Storage &db;

    Impl(msqlitecpp::v2::Storage &thedb);

    std::vector<std::string> getListOfAllPopulations();

    std::shared_ptr<PopulationData> getPopulationData(std::string popname, int period);
};


PopulationsLoader::PopulationsLoader(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{
}

std::vector<std::string> PopulationsLoader::getListOfAllPopulationsName()
{
    return p->getListOfAllPopulations();
}

vector<PopulationsLoader::PopulationData> PopulationsLoader::loadPopulationBaseData()
{
    vector<PopulationsLoader::PopulationData> data;

    PopulationAllQuery q(p->db);
    auto l = q.execute();

    std::transform(l.begin(), l.end(), std::back_inserter(data), [](std::pair<int, std::string> popdata) {
        PopulationsLoader::PopulationData d;
        d.a_name = std::get<0>(popdata);
        d.a_pop_name = std::get<1>(popdata);
        return d;
    });

    return data;
}

shared_ptr<PopulationsLoader::PopulationData> PopulationsLoader::getPopulationData(std::string popname, int period)
{
    return p->getPopulationData(popname, period);
}


/// Implementations

PopulationsLoader::Impl::Impl(msqlitecpp::v2::Storage &thedb)
        : db(thedb)
{
}

std::vector<std::string> PopulationsLoader::Impl::getListOfAllPopulations()
{
    PopulationAllQuery q(db);

    auto l = q.execute();
    std::vector<std::string> names;

    std::transform(l.begin(), l.end(), std::back_inserter(names), [](std::pair<int, std::string> popdata) {
        return std::get<1>(popdata);
    });
    return names;
}

shared_ptr<PopulationsLoader::PopulationData>
PopulationsLoader::Impl::getPopulationData(std::string popname, int period)
{
    auto pop = std::make_shared<PopulationsLoader::PopulationData>();
    PopulationParametersQuery query(db);

    *pop = query.executeForNamePeriod(popname, period);
    return pop;
}
