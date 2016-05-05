#ifndef NOIPC_H
#define NOIPC_H

#include <memory>
#include <vector>

class Vessel;
class Population;
class MemoryInfo;

class OutputMessage {
public:
    virtual ~OutputMessage() {}
};

class MoveShipOutputMessage : public OutputMessage {
public:
    MoveShipOutputMessage(int, void *) {}
};

class VesselLogbookOutputMessage : public OutputMessage
{
public:
    VesselLogbookOutputMessage(std::ostream &, unsigned int, Vessel *, const std::vector<Population *> &, std::vector <int> &) {}
};

class ExportVmslikeOutputMessage : public OutputMessage
{
public:
    ExportVmslikeOutputMessage(std::ostream &, unsigned int, void *) {}
};

class MoveVesselOutputMessage : public OutputMessage
{
public:
    MoveVesselOutputMessage(int, void *) {}
};

class OutputQueueManager {
public:
    void enqueue(std::shared_ptr<OutputMessage>){
    }
};

void inline initIpcQueue()
{
    std::cout << "Ipc Queue are disabled." << std::endl;
}

void inline finalizeIpcQueue()
{
}

void inline guiSendCurrentStep (unsigned int tstep)
{
}

void inline guiSendUpdateCommand (const std::string &filename, int tstep)
{
}

void inline guiSendMemoryInfo(const MemoryInfo &info)
{
}

void inline guiSendCapture(bool on)
{
}

void inline guiSendTerminalMessage(const string &ss)
{
}


#endif // NOIPC_H
