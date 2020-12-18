//
// Created by happycactus on 1/10/20.
//

#include "SimModel.h"
#include "comstructs.h"
#include "Calendar.h"
#include "shortestpath/GeoGraph.h"

struct SimModel::Impl {
    std::string namesimu;

    TimeStep tstep;
    int month = 0, quarter = 0, semester = 0;

    std::unique_ptr<displace::commons::Config> config;
    std::unique_ptr<displace::commons::Scenario> scenario;
    std::unique_ptr<Calendar> calendar;

    // TODO all those classes need to be transformed in special opaque classes
    GeoGraph geoGraph;
    DiffusionTree diffusionTree;
    std::vector<Node *> nodes;
    vector<Population *> populations;
    std::vector<Fishfarm *> fishfarms;
    vector<Windmill *> windmills;
    vector<Vessel *> vessels;
    vector<Ship *> ships;
    vector<Metier *> metiers;

    std::vector<int> graph_point_code_landscape;
    std::vector<int> graph_point_code_landscape_unique;
    vector<double> global_quotas_uptake;
    multimap<int, double> init_weight_per_szgroup;
    vector<vector<double> > species_interactions_mortality_proportion_matrix;
    // cached

    bool is_tacs;
    bool is_fishing_credits;
    bool is_discard_ban;
    bool is_grouped_tacs;
    bool is_benthos_in_numbers;
    bool is_benthos_in_longevity_classes;
    bool is_direct_killing_on_benthos;
    bool is_resuspension_effect_on_benthos;
    bool is_other_land_as_multiplier_on_sp;
    bool is_oth_land_per_metier;
};

SimModel::SimModel(std::string n)
        : p(spimpl::make_unique_impl<Impl>())
{
    p->namesimu = std::move(n);
}

std::string const &SimModel::nameSimu() const
{
    return p->namesimu;
}

void SimModel::setConfig(std::unique_ptr<displace::commons::Config> config)
{
    p->config = std::move(config);
}

void SimModel::setCalendar(std::unique_ptr<Calendar> calendar)
{
    p->calendar = std::move(calendar);
}

Calendar const &SimModel::calendar() const
{
    return *p->calendar;
}

void SimModel::initTimestep()
{
    p->tstep = TimeStep{0};
}

TimeStep SimModel::timestep() const
{
    return p->tstep;
}

void SimModel::nextTimestep()
{
    ++p->tstep;
//    p->tstep = TimeStep{p->tstep.value() + 1};
}

displace::commons::Config const &SimModel::config() const
{
    return *p->config;
}

void SimModel::setScenario(std::unique_ptr<displace::commons::Scenario> scenario)
{
    p->scenario = std::move(scenario);

    if (p->scenario->dyn_alloc_sce.option(Options::TACs)) {
        p->is_tacs = 1;
    } else {
        p->is_tacs = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::fishing_credits)) {
        p->is_fishing_credits = 1;
    } else {
        p->is_fishing_credits = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::discard_ban)) {
        p->is_discard_ban = 1;
    } else {
        p->is_discard_ban = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::groupedTACs)) {
        p->is_grouped_tacs = 1;
    } else {
        p->is_grouped_tacs = 0;
    }

    if (p->scenario->dyn_pop_sce.option(Options::modelBenthosInN)) {
        p->is_benthos_in_numbers = 1;
    } else {
        p->is_benthos_in_numbers = 0; // if not N then it impacts the benthos biomass by default
    }

    if (p->scenario->dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
        p->is_benthos_in_longevity_classes = 1;
    } else {
        p->is_benthos_in_longevity_classes = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::otherLandAsMultiplierOnSp)) {
        p->is_other_land_as_multiplier_on_sp = 1;
    }
    else {
        p->is_other_land_as_multiplier_on_sp = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::otherLandPerMetPerPop)) {
        p->is_oth_land_per_metier = 1;
    }
    else {
        p->is_oth_land_per_metier = 0;
    }

    

}

displace::commons::Scenario const &SimModel::scenario() const
{
    return *p->scenario;
}

void SimModel::setNodes(std::vector<Node *> nodes)
{
    p->nodes = std::move(nodes);
}

std::vector<Node *> const &SimModel::nodes() const
{
    return p->nodes;
}

std::vector<Node *> &SimModel::nodes()
{
    return p->nodes;
}

std::vector<Population *> const &SimModel::populations() const
{
    return p->populations;
}

std::vector<Population *> &SimModel::populations()
{
    return p->populations;
}

void SimModel::setPopulations(std::vector<Population *> population)
{
    p->populations = std::move(population);
}

vector<double> const &SimModel::globalQuotasUptake() const
{
    return p->global_quotas_uptake;
}

vector<double> &SimModel::globalQuotasUptake()
{
    return p->global_quotas_uptake;
}

void SimModel::setGlobalQuotasUptake(vector<double> v)
{
    p->global_quotas_uptake = std::move(v);
}

multimap<int, double> const &SimModel::initWeightPerSzgroup() const
{
    return p->init_weight_per_szgroup;
}

multimap<int, double> &SimModel::initWeightPerSzgroup()
{
    return p->init_weight_per_szgroup;
}

void SimModel::setInitWeightPerSzgroup(multimap<int, double> m)
{
    p->init_weight_per_szgroup = std::move(m);
}

vector<vector<double> > const &SimModel::species_interactions_mortality_proportion_matrix() const
{
    return p->species_interactions_mortality_proportion_matrix;
}

vector<vector<double> > &SimModel::species_interactions_mortality_proportion_matrix()
{
    return p->species_interactions_mortality_proportion_matrix;
}

void SimModel::set_species_interactions_mortality_proportion_matrix(vector<vector<double> > v)
{
    p->species_interactions_mortality_proportion_matrix = std::move(v);
}

void SimModel::setVessels(std::vector<Vessel *> vessels)
{
    p->vessels = std::move(vessels);
}

std::vector<Vessel *> &SimModel::vessels()
{
    return p->vessels;
}

std::vector<Vessel *> const &SimModel::vessels() const
{
    return p->vessels;
}

void SimModel::setShips(std::vector<Ship *> ships)
{
    p->ships = std::move(ships);
}

std::vector<Ship *> &SimModel::ships()
{
    return p->ships;
}

std::vector<Ship *> const &SimModel::ships() const
{
    return p->ships;
}

void SimModel::setMetiers(std::vector<Metier *> metiers)
{
    p->metiers = std::move(metiers);
    ////p->metiers = metiers;
}

std::vector<Metier *> &SimModel::metiers()
{
    return p->metiers;
}

std::vector<Metier *> const &SimModel::metiers() const
{
    return p->metiers;
}

void SimModel::setQuarter(int quarter)
{
    p->quarter = quarter;
}

int SimModel::quarter() const
{
    return p->quarter;
}

void SimModel::setMonth(int month)
{
    p->month = month;
}

int SimModel::month() const
{
    return p->month;
}

void SimModel::setSemester(int semester)
{
    p->semester = semester;
}

int SimModel::semester() const
{
    return p->semester;
}

void SimModel::set_graph_point_code_landscape(std::vector<int> v)
{
    p->graph_point_code_landscape_unique = v;
    p->graph_point_code_landscape = std::move(v);

    sort(p->graph_point_code_landscape_unique.begin(), p->graph_point_code_landscape_unique.end());
    auto it = std::unique(p->graph_point_code_landscape_unique.begin(), p->graph_point_code_landscape_unique.end());
    p->graph_point_code_landscape_unique.resize(std::distance(p->graph_point_code_landscape_unique.begin(), it));
}

std::vector<int> const &SimModel::graph_point_code_landscape() const
{
    return p->graph_point_code_landscape;
}

std::vector<int> const &SimModel::graph_point_code_landscape_unique() const
{
    return p->graph_point_code_landscape_unique;
}

std::vector<int> &SimModel::graph_point_code_landscape()
{
    return p->graph_point_code_landscape;
}

void SimModel::setWindmills(vector<Windmill *> windmills)
{
    p->windmills = std::move(windmills);
}

vector<Windmill *> &SimModel::windmills()
{
    return p->windmills;
}

vector<Windmill *> const &SimModel::windmills() const
{
    return p->windmills;
}

void SimModel::setFishFarms(vector<Fishfarm *> fishfarms)
{
    p->fishfarms = std::move(fishfarms);
}

std::vector<Fishfarm *> &SimModel::fishfarms()
{
    return p->fishfarms;
}

std::vector<Fishfarm *> const &SimModel::fishfarms() const
{
    return p->fishfarms;
}

DiffusionTree &SimModel::diffusionTree()
{
    return p->diffusionTree;
}

void SimModel::setGeoGraph(GeoGraph geoGraph)
{
    p->geoGraph = std::move(geoGraph);
}

GeoGraph const &SimModel::geoGraph() const
{
    return p->geoGraph;
}

bool SimModel::is_tacs() const
{
    return p->is_tacs;
}

bool SimModel::is_fishing_credits() const
{
    return p->is_fishing_credits;
}

bool SimModel::is_discard_ban() const
{
    return p->is_discard_ban;
}

bool SimModel::is_grouped_tacs() const
{
    return p->is_grouped_tacs;
}

bool SimModel::is_benthos_in_numbers() const
{
    return p->is_benthos_in_numbers;
}

bool SimModel::is_benthos_in_longevity_classes() const
{
    return p->is_benthos_in_longevity_classes;
}

bool SimModel::is_direct_killing_on_benthos() const
{
    return p->is_direct_killing_on_benthos;
}

bool SimModel::is_resuspension_effect_on_benthos() const
{
    return p->is_resuspension_effect_on_benthos;
}

bool SimModel::is_other_land_as_multiplier_on_sp() const
{
    return p->is_other_land_as_multiplier_on_sp;
}

bool SimModel::is_oth_land_per_metier() const
{
    return p->is_oth_land_per_metier;
}


void SimModel::initRandom(std::string const &namesimu)
{

    size_t iii = 0;
    // detect a integer in the name of the simu
    for (; iii < namesimu.length(); iii++) { if (isdigit(namesimu[iii])) { break; }}

    // remove the first chars, which aren't digits
    std::string str = namesimu.substr(iii, namesimu.length() - iii);

    // convert the remaining text to an integer
    int a_seed = atoi(str.c_str());

    // print the result
    std::cout << "The random seed is set to: " << a_seed << std::endl;

    // set always the same seed for a given simu name (that should include a integer with pattern s100 or sim100 or simu100...)
    srand(a_seed);
}

