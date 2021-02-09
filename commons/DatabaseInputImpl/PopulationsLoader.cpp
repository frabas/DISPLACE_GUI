
#include "PopulationsLoader.h"
#include "Population.h"
#include "queries/PopulationQuery.h"
#include "queries/PopulationParametersQuery.h"
#include "queries/PopulationGroupsQuery.h"

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

vector<PopulationsLoader::PopulationData> PopulationsLoader::loadPopulationBaseData(int period)
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

    PopulationParametersQuery parametersQuery(p->db, period);
    parametersQuery.execute(data);

    PopulationGroupsQuery groupsQuery(p->db, period);
    groupsQuery.execute(data);

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
