#ifndef IPCMSGTYPES_H
#define IPCMSGTYPES_H

enum IpcMessageTypes {
    ForceExit,
    GenericConsole,
    VesselLogbook,
    ExportVms,
    MoveVessel
};

namespace displace {
namespace ipc {

struct MoveVesselMessage {
    unsigned int tstep, tstep_dep;
    unsigned int idx;
    double x,y,course,cumfuelcons;
    int state;
};

struct VesselLogbookMessage {
    unsigned int tstep, tstepdep;
    int rtbb, node, idx;
    double cumstm, timeatsea,cumfcons,travdist, revenue, revenue_from_av_prices, fuelcost, gav,gav2;
};

}
}

#endif // IPCMSGTYPES_H
