#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>

class SQLiteResultsStorage;

class SQLiteTable
{
    std::weak_ptr<SQLiteResultsStorage> mdb;
    std::string mName;
public:
    SQLiteTable(std::shared_ptr<SQLiteResultsStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    virtual bool create();
    //virtual bool update() = 0;

    std::string name() const { return mName; }

protected:
    std::shared_ptr<SQLiteResultsStorage> db();
    // helper functions here

    virtual std::string getCreateDefinition() { return std::string(); }
};

#endif // SQLITETABLE_H
