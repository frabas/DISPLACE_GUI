#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>
#include <tuple>
#include <sstream>

#include "sqlitefielddef.h"

namespace sqlite {

class SQLiteStorage;
template <typename ...FIELDTYPE>
using TableDef = std::tuple<FieldDef<FIELDTYPE>...>;


template <size_t I, typename ...Ts>
inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    return std::string();
};

template <size_t I = 0, typename ...Ts>
inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto & field = std::get<I>(def);
    return field.name() + " " + field.sqlType() + field.sqlAttributes()
           + (I == sizeof...(Ts)-1 ? "" : ",") + "\n" +
                                                                                    buildSqlCreateString<I+1, Ts...>(def);
};

class SQLiteTable
{
public:
    class Statement;

private:
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;

protected:

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertFieldList(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertFieldList(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(def);
        return field.name() + (I == sizeof...(Ts)-1 ? "" : ",") + buildSqlInsertFieldList<I+1, Ts...>(def);
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertValuesListPlaceholder(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertValuesListPlaceholder(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(def);
        std::ostringstream ss;
        ss << "?" << (I+1) << (I == sizeof...(Ts)-1 ? "" : ",") << buildSqlInsertValuesListPlaceholder<I+1, Ts...>(def);
        return ss.str();
     //   return std::string("?") + (I == sizeof...(Ts)-1 ? "" : ",") + buildSqlInsertValuesListPlaceholder<I+1, Ts...>(def);
    }

    template <typename T>
    void bindValue(Statement *stmt, int idx, T value) {
        static_assert(sizeof(T) == 0, "Generic version of bindValue is undefined");
    }

    template <size_t I, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
        (void) values; (void)stmt;
    }
    template <size_t I, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
        bindValue(stmt, I+1, std::get<I>(values));
        bindAllValues<I+1,Ts...>(stmt, values);
    }

public:
    SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    bool createFromSQLString (std::string query);

    template <typename ...Ts>
    bool create (std::tuple<Ts...> def) {
        std::ostringstream ss;
        ss << "CREATE TABLE " << mName << " ("
                                       << buildSqlCreateString(def) << ");";
        return createFromSQLString(ss.str());
    }
    //virtual bool update() = 0;

    std::string name() const { return mName; }

    std::shared_ptr<Statement> newStatement(std::string query);
    bool execute(Statement *stmt);

    template <typename ...Ts, typename ...Us>
    bool insert (std::tuple<Ts...> def, std::tuple<Us...> values) {
        std::ostringstream ss;
        ss << "INSERT INTO " << mName << "("
                                      << buildSqlInsertFieldList<0>(def)
                                      << ") VALUES ("
                                      << buildSqlInsertValuesListPlaceholder<0>(values)
                                      << ");";
        auto stmt = newStatement(ss.str());
        bindAllValues<0>(stmt.get(), values);
        return execute(stmt.get());
    }

    template <typename ...Ts>
    static SQLiteTable create (std::shared_ptr<SQLiteStorage> db, std::string name, std::tuple<Ts...> def) {
        SQLiteTable table(db, name);

        table.create(def);

        return table;
    }

protected:
    std::shared_ptr<SQLiteStorage> db();
    // helper functions here

};

template <>
void SQLiteTable::bindValue<int> (SQLiteTable::Statement *stmt, int idx, int value);

template <>
void SQLiteTable::bindValue<std::string> (SQLiteTable::Statement *stmt, int idx, std::string value);

template <>
void SQLiteTable::bindValue<double> (SQLiteTable::Statement *stmt, int idx, double value);


template <typename ...FIELDTYPE>
inline TableDef<FIELDTYPE...> makeTableDef()
{
}

} // ns sqlite

#endif // SQLITETABLE_H
