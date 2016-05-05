#ifndef NOIPC_H
#define NOIPC_H

#include <memory>
#include <vector>

class Vessel;
class Population;

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

#endif // NOIPC_H
