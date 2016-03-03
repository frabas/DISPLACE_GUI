//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_PLAYGROUND_H
#define DISPLACE_PLAYGROUND_H

#include <memory>

class MapGraph;

class Playground {
public:
    explicit Playground();
    ~Playground() noexcept = default;

private:
    std::unique_ptr<MapGraph> mGraph;
};


#endif //DISPLACE_PLAYGROUND_H
