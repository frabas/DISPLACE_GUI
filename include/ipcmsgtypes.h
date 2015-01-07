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

}
}

#endif // IPCMSGTYPES_H
