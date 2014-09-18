#ifndef OPENSEAMAPADAPTER_H
#define OPENSEAMAPADAPTER_H

#include "qmapcontrol_global.h"
#include "tilemapadapter.h"

namespace qmapcontrol {

class QMAPCONTROL_EXPORT  OpenSeaMapAdapter : public TileMapAdapter
{
    Q_OBJECT

public:
    OpenSeaMapAdapter();
    virtual ~OpenSeaMapAdapter();
};

} // namespace

#endif // OPENSEAMAPADAPTER_H
