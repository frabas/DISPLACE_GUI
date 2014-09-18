#include "openseamapadapter.h"

namespace qmapcontrol {

// Urls taken from: http://wiki.openseamap.org/wiki/OpenSeaMap-dev:Server

OpenSeaMapAdapter::OpenSeaMapAdapter()
 : TileMapAdapter("tiles.openseamap.org", "/seamark/%1/%2/%3.png", 256, 0, 17)
{
}

OpenSeaMapAdapter::~OpenSeaMapAdapter()
{

}


} // namespace
