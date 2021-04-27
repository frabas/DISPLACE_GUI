//
// Created by happycactus on 12/28/19.
//

#ifndef DISPLACE_MODELLOADER_H
#define DISPLACE_MODELLOADER_H

#include "modeltypes.h"
#include "comstructs.h"

#include <vector>
#include <map>
#include <msqlitecpp/utils/spimpl.h>

class Benthos;

class PopSceOptions;

class DynAllocOptions;

class ConsistencyTestFailed : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SimModel;

/**
 * @brief Implements an abstract interface to a loader for the whole model
 */
class ModelLoader {
public:
    explicit ModelLoader(std::shared_ptr<SimModel> model);

    virtual ~ModelLoader() = default;

    std::vector<Benthos *> loadBenthos(
            PopSceOptions const &dyn_pop_sce,
            DynAllocOptions const &dyn_alloc_sce,
            std::string const &biolsce,
            std::string const &fleetsce);

    bool loadConfig();

    bool loadScenario();

    virtual void loadCalendar() = 0;

    virtual void loadShips() = 0;

    virtual void loadVessels(int year, int month, int quarter, int semester) = 0;

    bool loadFishFarms();

    bool loadWindmills();

    virtual void loadMetiers(int year, int month, int quarter, int semester) = 0;

    bool loadNodesAndGraphs();

    virtual bool loadPopulations(int year) = 0;

    struct BenthosData {
        int int1;       // is_benthos_in_numbers
        int int2;       // is_benthos_in_longevity_classes
        std::multimap<int, double> mmapidparam1;
        std::multimap<int, double> mmapidparam2;
        std::multimap<int, double> mmapidparam3;
        std::multimap<int, double> mmapidparam4;
        std::multimap<int, double> mmapidparam5;
        std::multimap<int, double> mmapidparam6;
        std::multimap<int, double> mmapidparam7;
        std::multimap<int, double> mmapidparam8;
    };

    SimModel &model();

    SimModel const &model() const;

    // TODO: consider to calculate semester and quarter from month
    std::string quarterString() const;

    std::string monthString() const;

    std::string semesterString() const;

protected:
    virtual bool loadConfigImpl(int &nbpops,
                                int &nbbenthospops,
                                std::vector<int> &implicit_pops,
                                std::vector<int> &implicit_pops_level2,
                                std::vector<int> &grouped_tacs,
                                std::vector<int> &nbcp_coupling_pops,
                                std::vector<double> &calib_oth_landings,
                                std::vector<double> &calib_w,
                                std::vector<double> &calib_cpue,
                                std::vector<types::NodeId> &interesting_harbours) = 0;

    virtual bool loadScenarioImpl(displace::commons::Scenario &scenario) = 0;

    virtual bool loadNodesAndGraphsDataImpl() = 0;

    virtual bool loadFishFarmsImpl() = 0;

    virtual bool loadWindmillsImpl() = 0;

    virtual BenthosData loadBenthosData(
            PopSceOptions const &dyn_pop_sce,
            DynAllocOptions const &dyn_alloc_sce,
            std::string const &biolsce,
            std::string const &fleetsce) = 0;

    void doBenthosConsistencyTest(std::vector<Benthos *> const &benthoss);

    bool doNodesAndGraphConsistencyTest();

    void ensure(bool condition)
    {
        if (!condition) {
            throw std::logic_error("Precondition not met.");
        }
    }

private:
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
};


#endif //DISPLACE_MODELLOADER_H
