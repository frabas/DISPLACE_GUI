#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>

class SQLiteTable
{
    std::string mName;
public:
    SQLiteTable(std::string name);
    virtual ~SQLiteTable() noexcept;

    virtual bool create() = 0;
    virtual bool update() = 0;

protected:
    // helper functions here
};

#endif // SQLITETABLE_H
