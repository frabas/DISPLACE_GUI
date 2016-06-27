#ifndef NODEPENALTY_H
#define NODEPENALTY_H

#include <vector>

namespace displace {

class NodePenalty {
public:
    int nodeId;

    bool closed;
    std::vector<int> metiers;
    bool q[4];

    int polyId;
};

}

#endif // NODEPENALTY_H
