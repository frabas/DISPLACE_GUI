#include "modelmetadataaccessor.h"

#include "tables/metadatatable.h"

ModelMetadataAccessor::ModelMetadataAccessor(std::shared_ptr<MetadataTable> t)
    : meta(t)
{
}

void ModelMetadataAccessor::setLastTStep(size_t tstep)
{
    std::ostringstream ss;
    ss << tstep;
    meta->setMetadata("lastTStep", ss.str());
}

size_t ModelMetadataAccessor::lastTStep() const
{
    return meta->getMetadataAs<size_t>("lastTStep");
}


