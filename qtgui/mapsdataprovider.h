#ifndef MAPSDATAPROVIDER_H
#define MAPSDATAPROVIDER_H

#include <memory>

class MapsDataProvider
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    MapsDataProvider();
    ~MapsDataProvider() noexcept;

};

#endif // MAPSDATAPROVIDER_H
