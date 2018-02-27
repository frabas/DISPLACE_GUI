#ifndef MODELMETADATAACCESSOR_H
#define MODELMETADATAACCESSOR_H

#include "commons_global.h"

#include <memory>

class MetadataTable;

class COMMONSSHARED_EXPORT ModelMetadataAccessor
{
    std::shared_ptr<MetadataTable> meta;
public:
    explicit ModelMetadataAccessor(std::shared_ptr<MetadataTable>);

    void setLastTStep(size_t tstep);
    size_t lastTStep() const;
};

#endif // MODELMETADATAACCESSOR_H
