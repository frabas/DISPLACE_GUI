//
// Created by happycactus on 12/28/19.
//

#ifndef DISPLACE_MODELLOADER_H
#define DISPLACE_MODELLOADER_H

#include <vector>
#include <map>
#include <msqlitecpp/utils/spimpl.h>

class Benthos;

class PopSceOptions;

class DynAllocOptions;

class ConsistencyTestFailed : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @brief Implements an abstract interface to a loader for the whole model
 */
class ModelLoader {
public:
    ModelLoader();

    virtual ~ModelLoader() = default;

    std::vector<Benthos *> loadBenthos(
            std::vector<int> const &graph_point_code_landscape,
            PopSceOptions const &dyn_pop_sce,
            DynAllocOptions const &dyn_alloc_sce,
            std::string const &biolsce,
            std::string const &fleetsce);

    struct BenthosData {
        int int1;
        int int2;
        std::multimap<int, double> mmapidparam1;
        std::multimap<int, double> mmapidparam2;
        std::multimap<int, double> mmapidparam3;
        std::multimap<int, double> mmapidparam4;
        std::multimap<int, double> mmapidparam5;
        std::multimap<int, double> mmapidparam6;
        std::multimap<int, double> mmapidparam7;
        std::multimap<int, double> mmapidparam8;
    };

protected:
    virtual BenthosData loadBenthosData(
            PopSceOptions const &dyn_pop_sce,
            DynAllocOptions const &dyn_alloc_sce,
            std::string const &biolsce,
            std::string const &fleetsce) = 0;

    void doBenthosConsistencyTest(std::vector<Benthos *> const &benthoss);

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
