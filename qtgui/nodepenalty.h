#ifndef NODEPENALTY_H
#define NODEPENALTY_H

#include <vector>

namespace displace {

class NodePenalty {
public:
    int nodeId;

    double nbOfDaysClosed;
    std::vector<int> metiers;
    std::vector<bool> months;
    std::vector<int> vesSizes;
    std::vector<int> Nations;
    bool q[4];

    int polyId;
};

}

#endif // NODEPENALTY_H
