#include "metadatatable.h"

#include <sqlitestatement.h>
#include "sqlitestatementformatters.h"
#include "sqlitefieldsop.h"
#include "utils/make_unique.h"

struct MetadataTable::Impl
{
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Text>,FieldDef<FieldType::Text>> insertStatement;
    SQLiteStatement selectStatement;
};

MetadataTable::MetadataTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(utils::make_unique<Impl>())
{
}

MetadataTable::~MetadataTable() noexcept = default;

void MetadataTable::lazyInit()
{
    if (!p->init) {
        p->init = true;
        p->insertStatement = prepareInsertOrReplace(std::make_tuple(fldKey, fldValue));

        auto t = statements::Select(name(), fldValue).where(op::eq(fldKey));
        p->selectStatement = std::move(SQLiteStatement(db(),t));
    }
}

void MetadataTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldKey, fldValue));
    createIndex("KeyIndex", std::make_tuple(fldKey));
}


void MetadataTable::setMetadata(std::string key, std::string value)
{
    std::unique_lock<std::mutex> m(p->mutex);
    lazyInit();

    insert(p->insertStatement, std::make_tuple(key, value));
}

std::string MetadataTable::getMetadata (std::string key)
{
    std::unique_lock<std::mutex> m(p->mutex);
    lazyInit();

    std::string v;
    p->selectStatement.bind(1, key);
    p->selectStatement.execute([&v, this]() { v = p->selectStatement.getStringValue(0); return true; });

    return v;
}
