#include "mapsdataprovider.h"

struct MapsDataProvider::Impl
{

};

MapsDataProvider::MapsDataProvider()
    : p(std::make_unique<Impl>())
{

}

MapsDataProvider::~MapsDataProvider() noexcept = default;

