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

void ModelMetadataAccessor::setNbMets(int n)
{
    std::ostringstream ss;
    ss << n;
    meta->setMetadata("nbMets", ss.str());
}

int ModelMetadataAccessor::nbMets() const
{
    return meta->getMetadataAs<size_t>("nbMets");
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

std::vector<int> ModelMetadataAccessor::getVectorOfInt(std::string key)
{
    auto stringvector = meta->getMetadata(key);

    std::stringstream iss( stringvector );

    int number;
    std::vector<int> results;
    while ( iss >> number )
      results.push_back( number );

    return results;
}

void ModelMetadataAccessor::setVectorOfInt(std::string key, const std::vector<int> &vector)
{
    std::stringstream result;
    std::copy(vector.begin(), vector.end(), std::ostream_iterator<int>(result, " "));

    meta->setMetadata(key, result.str());
}
