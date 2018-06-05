//
// Created by Federico Fuga on 05/06/18.
//

#include "fishfarmsdeftable.h"
#include "Fishfarm.h"

#include <insertstatement.h>
#include <createstatement.h>

using namespace sqlite;

struct FishFarmsDefTable::Impl {
    std::shared_ptr<SQLiteStorage> db;
    std::string tableName;

    FieldDef<FieldType::Integer> fId{"Id", PrimaryKey};
    FieldDef<FieldType::Text> fName{"name"};
    FieldDef<FieldType::Integer> fType{"type"};
    FieldDef<FieldType::Real> fX{"x"};
    FieldDef<FieldType::Real> fY{"y"};

    InsertStatement<
            decltype(fId),
            decltype(fName),
            decltype(fType),
            decltype(fX),
            decltype(fY)
    > insertStatement;

    explicit Impl(std::shared_ptr<SQLiteStorage> s, std::string name)
            : db(s), tableName(name),
              insertStatement(fId, fName, fType, fX, fY)
    {
        create();
        initStatements();
    }

    void drop() {
        if (db->tableExists(tableName))
            db->dropTable(tableName);
        create();
    }

    void create() {
        if (db->tableExists(tableName))
            return;

        CreateTableStatement<
                decltype(fId),
                decltype(fName),
                decltype(fType),
                decltype(fX),
                decltype(fY)
        > createTableStatement(fId, fName, fType, fX, fY);

        createTableStatement.attach(db, tableName);
        createTableStatement.execute();
    }

    void initStatements() {
        insertStatement.replaceOnConflict();
        insertStatement.attach(db, tableName);
    };

    void insert(const Fishfarm &fishfarm) {
        insertStatement.insert(
                fishfarm.get_name(),
                fishfarm.get_stringname(),
                fishfarm.get_farmtype(),
                fishfarm.get_x(),
                fishfarm.get_y()
                );
    }
};

FishFarmsDefTable::FishFarmsDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
        : p(std::make_unique<Impl>(db, name))
{

}

FishFarmsDefTable::~FishFarmsDefTable() noexcept = default;

void FishFarmsDefTable::dropAndCreate()
{
    p->drop();
    p->create();
}

void FishFarmsDefTable::insertDef(const Fishfarm &fishfarm)
{
    p->insert(fishfarm);
}
