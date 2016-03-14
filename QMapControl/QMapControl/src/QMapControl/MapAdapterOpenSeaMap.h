#ifndef MAPADAPTEROPENSEAMAP_H
#define MAPADAPTEROPENSEAMAP_H

#pragma once

// Local includes.
#include "qmapcontrol_global.h"
#include "MapAdapterTile.h"

namespace qmapcontrol
{

class QMAPCONTROL_EXPORT MapAdapterOpenSeaMap : public MapAdapterTile
{
    Q_OBJECT

public:
    MapAdapterOpenSeaMap(QObject *parent = 0);

    //! Destructor.
    ~MapAdapterOpenSeaMap() { } /// = default; @todo re-add once MSVC supports default/delete syntax.

private:
    //! Disable copy constructor.
    MapAdapterOpenSeaMap(const MapAdapterOpenSeaMap&); /// @todo remove once MSVC supports default/delete syntax.

    //! Disable copy assignment.
    MapAdapterOpenSeaMap& operator=(const MapAdapterOpenSeaMap&); /// @todo remove once MSVC supports default/delete syntax.
};

}

#endif // MAPADAPTEROPENSEAMAP_H
