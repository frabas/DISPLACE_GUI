#ifndef NODEPENALTY_H
#define NODEPENALTY_H

#include <vector>

namespace displace {

class NodePenalty {
public:
    int nodeId;

    bool closed;
    std::vector<int> metiers;
    std::vector<bool> months;
    std::vector<bool> vesSizes;
    bool q[4];

    int polyId;
};

}

#endif // NODEPENALTY_H
