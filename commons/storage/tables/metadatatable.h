#ifndef METADATATABLE_H
#define METADATATABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

#include <boost/lexical_cast.hpp>


class COMMONSSHARED_EXPORT  MetadataTable : public SQLiteTable
{
    struct Impl;
    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Text> fldKey = makeFieldDef("Key",FieldType::Text()).primaryKey();
    FieldDef<FieldType::Text> fldValue = makeFieldDef("Value",FieldType::Text());

    void lazyInit();
public:
    MetadataTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    ~MetadataTable() noexcept;

    void dropAndCreate();

    void setMetadata (std::string key, std::string value);
    std::string getMetadata (std::string key);

    template <typename T>
    T getMetadataAs(std::string key) {
        return boost::lexical_cast<T>(getMetadata(key));
    }
};

#endif // METADATATABLE_H
