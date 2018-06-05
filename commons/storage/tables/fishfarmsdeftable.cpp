//
// Created by Federico Fuga on 05/06/18.
//

#include "fishfarmsdeftable.h"
#include "Fishfarm.h"

#include <insertstatement.h>
#include <createstatement.h>
#include <selectstatement.h>

using namespace sqlite;

struct FishFarmsDefTable::Impl {
    std::shared_ptr<SQLiteStorage> db;
    std::string tableName;

    FieldDef<FieldType::Integer> fId{"Id", PrimaryKey};
    FieldDef<FieldType::Integer> fNodeId{"NodeId", NotNull};
    FieldDef<FieldType::Text> fName{"name"};
    FieldDef<FieldType::Integer> fType{"type"};
    FieldDef<FieldType::Real> fX{"x"};
    FieldDef<FieldType::Real> fY{"y"};

    InsertStatement<
            decltype(fId),
            decltype(fNodeId),
            decltype(fName),
            decltype(fType),
            decltype(fX),
            decltype(fY)
    > insertStatement;
    SelectStatement<
            decltype(fId),
            decltype(fNodeId),
            decltype(fName),
            decltype(fType),
            decltype(fX),
            decltype(fY)
    > selectAllFishfarms;

    explicit Impl(std::shared_ptr<SQLiteStorage> s, std::string name)
            : db(s), tableName(name),
              insertStatement(fId, fNodeId, fName, fType, fX, fY),
              selectAllFishfarms(fId, fNodeId, fName, fType, fX, fY)
    {
        create();
        initStatements();
    }

    void drop()
    {
        if (db->tableExists(tableName)) {
            db->dropTable(tableName);
        }
        create();
    }

    void create()
    {
        if (db->tableExists(tableName)) {
            return;
        }

        CreateTableStatement<
                decltype(fId),
                decltype(fNodeId),
                decltype(fName),
                decltype(fType),
                decltype(fX),
                decltype(fY)
        > createTableStatement(fId, fNodeId, fName, fType, fX, fY);

        createTableStatement.attach(db, tableName);
        createTableStatement.execute();
    }

    void initStatements()
    {
        insertStatement.replaceOnConflict();
        insertStatement.attach(db, tableName);

        selectAllFishfarms.attach(db, tableName);
        selectAllFishfarms.prepare();
    };

    void insert(const Fishfarm &fishfarm)
    {
        insertStatement.insert(
                fishfarm.get_name(),
                fishfarm.get_loc_ff()->get_idx_node().toIndex(),
                fishfarm.get_stringname(),
                fishfarm.get_farmtype(),
                fishfarm.get_x(),
                fishfarm.get_y()
        );
    }

    void getAllFishfarms(std::function<bool(int nodeId, std::shared_ptr<Fishfarm> fishfarm)> func)
    {
        selectAllFishfarms.exec([&func](int id,
                                   int nodeId,
                                   std::string name,
                                   int type,
                                   double x, double y) {
            auto ff = std::make_shared<Fishfarm>(nullptr, id, name, type);
            ff->set_x(x);
            ff->set_y(y);
            return func(nodeId, ff);
        });
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

void FishFarmsDefTable::getAllFishfarms(FishFarmsDefTable::FishfarmSelectFunc func)
{
    p->getAllFishfarms(func);
}
