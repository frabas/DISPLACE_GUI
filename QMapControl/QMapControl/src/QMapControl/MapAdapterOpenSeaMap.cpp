#include "MapAdapterOpenSeaMap.h"


namespace qmapcontrol
{

namespace
{
    /// @todo remove once MSVC supports initializer lists.
    std::set<projection::EPSG> supportedProjections()
    {
        std::set<projection::EPSG> projections;
        projections.insert(projection::EPSG::SphericalMercator);
        return projections;
    }
}

MapAdapterOpenSeaMap::MapAdapterOpenSeaMap(QObject* parent)
    : MapAdapterTile(QUrl("http://tiles.openseamap.org/seamark/%zoom/%x/%y.png"), supportedProjections(), 0, 17, 0, false, parent) /// @todo remove once MSVC supports initializer lists.
{
}


}

