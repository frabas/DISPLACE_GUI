#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>

class SQLiteStorage;

class SQLiteTable
{
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;
public:
    SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    virtual bool create();
    //virtual bool update() = 0;

    std::string name() const { return mName; }

protected:
    std::shared_ptr<SQLiteStorage> db();
    // helper functions here

    virtual std::string getCreateDefinition() { return std::string(); }
};

#endif // SQLITETABLE_H
