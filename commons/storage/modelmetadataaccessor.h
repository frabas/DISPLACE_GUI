#ifndef MODELMETADATAACCESSOR_H
#define MODELMETADATAACCESSOR_H

#include "commons_global.h"

#include <memory>
#include <vector>
#include <string>

#include<iterator>

class MetadataTable;

class  ModelMetadataAccessor
{
    std::shared_ptr<MetadataTable> meta;
public:
    explicit ModelMetadataAccessor(std::shared_ptr<MetadataTable>);

    void setLastTStep(size_t tstep);
    size_t lastTStep() const;

    void setNbPops(int n);
    int nbPops() const;

    void setNbMets(int n);
    int nbMets() const;
    
    void setNbSizes(int n);
    int nbSize() const;

    void setNbBenthos(int n);
    int nbBenthos();

    std::vector<int> getVectorOfInt (std::string key);
    void setVectorOfInt(std::string key, const std::vector<int> &vector);
};

#endif // MODELMETADATAACCESSOR_H
