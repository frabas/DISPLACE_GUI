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

    void setNbPops(int n);
    int nbPops() const;

    void setNbSizes(int n);
    int nbSize() const;

    void setNbBenthos(int n);
    int nbBenthos();

    std::vector<int> getVectorOfInt (std::string key);
    void setVectorOfInt(std::string key, const std::vector<int> &vector);
};

#endif // MODELMETADATAACCESSOR_H
