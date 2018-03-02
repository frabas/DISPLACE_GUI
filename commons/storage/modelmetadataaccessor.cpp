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

void ModelMetadataAccessor::setNbPops(int n)
{
    std::ostringstream ss;
    ss << n;
    meta->setMetadata("nbPops", ss.str());
}

int ModelMetadataAccessor::nbPops() const
{
    return meta->getMetadataAs<size_t>("nbPops");
}

void ModelMetadataAccessor::setNbSizes(int n)
{
    std::ostringstream ss;
    ss << n;
    meta->setMetadata("nbSizes", ss.str());
}

int ModelMetadataAccessor::nbSize() const
{
    return meta->getMetadataAs<size_t>("nbSizes");
}

void ModelMetadataAccessor::setNbBenthos(int n)
{
    std::ostringstream ss;
    ss << n;
    meta->setMetadata("nbBenthos", ss.str());
}

int ModelMetadataAccessor::nbBenthos()
{
    return meta->getMetadataAs<size_t>("nbBenthos");
}
