#ifndef MODELTYPES_H
#define MODELTYPES_H

#include "StrongType.h"
#include "idtypes.h"

#include <vector>

namespace types {

struct TStepTag {};
using tstep_t = utils::StrongType<uint16_t, TStepTag>;

struct EnvironmentData {
    types::NodeId nodeId;
    tstep_t tstep;

    int marineLandscape;
    double salinity;
    double sst;
    double wind;
    double nitrogen;
    double phosphorus;
    double oxygen;
    double dissolvedcarbon;
};

struct NodesPopData {
    tstep_t tstep;
    types::NodeId nodeId;

    std::vector<double> totN, totW, cumC, cumD, impact;
};

struct NodesBenthosData {
    tstep_t tstep;
    types::NodeId nodeId;
    
    std::vector<double> mBenthosBiomass, mBenthosNumber, mBenthosMeanweight, mBenthosBiomassOverK, mBenthosNumberOverK;
};

}

#endif // MODELTYPES_H
