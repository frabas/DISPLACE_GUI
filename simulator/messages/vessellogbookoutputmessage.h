#ifndef VESSELLOGBOOKOUTPUTMESSAGE_H
#define VESSELLOGBOOKOUTPUTMESSAGE_H

#include <outputmessage.h>
#include <ostream>
#include <string>
#include <vector>

class Vessel;
class Population;

class VesselLogbookOutputMessage : public OutputMessage
{
public:
    VesselLogbookOutputMessage(std::ostream &strm, unsigned int _tstep, Vessel *v, const std::vector<Population *> &populations);

    virtual int getType() const {
        return 4;
    }

    bool send(std::ostream &);
    virtual int sendBinary(void *buffer, size_t maxlen);

private:
    std::ostream &loglike;

    unsigned int tstep, tstepdep;
    int rtbb, node, idx;
    double cumstm, timeatsea,cumfcons,travdist, revenue, revenue_from_av_prices, fuelcost, gav,gav2;
    std::string name, freq_metiers, length_class;

    std::vector<double> cumul;
};

#endif // VESSELLOGBOOKOUTPUTMESSAGE_H
