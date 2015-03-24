#ifndef GRAPHNODEEXTRA_H
#define GRAPHNODEEXTRA_H

#include <dtree/dtnodeextra.h>

namespace dtree {

class GraphNodeExtra : public NodeExtra
{
public:
    GraphNodeExtra();

    bool isUnconnected() const { return mUnconnected; }
    void setUnconnected(bool s) { mUnconnected = s; }


private:
    bool mUnconnected = true;
};


}

#endif // GRAPHNODEEXTRA_H
