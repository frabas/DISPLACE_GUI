#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>
#include <tuple>

#include "storage/sqlitefielddef.h"

namespace sqlite {

class SQLiteStorage;
template <typename ...FIELDTYPE>
using TableDef = std::tuple<FieldDef<FIELDTYPE>...>;

namespace detail {

template <int N>
struct seq {};

template <int N, int ... S>
struct gens : gens<N-1, N-1, S...> {};

template <int ...S>
struct gens<0, S...> {
    using type = seq<S...>;
};

template <int ...S, typename ...FIELDTYPE>
std::string buildSqlCreateString(detail::seq<S...>, std::tuple<FIELDTYPE...> && def) {
    auto &z = std::get<S>(def);
    return z.name() + " " + z.sqlType + " ";
}

template <typename ...FIELDTYPE>
std::string buildSqlCreateString(std::tuple<FIELDTYPE...> && def) {
    return buildSqlCreateString(typename detail::gens<sizeof...(def)>::type());
}
} // ns details


class SQLiteTable
{
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;
public:
    SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    virtual bool create();
    bool createFromSQLString (std::string query);
    //virtual bool update() = 0;

    std::string name() const { return mName; }

protected:
    std::shared_ptr<SQLiteStorage> db();
    // helper functions here

    virtual std::string getCreateDefinition() { return std::string(); }

    template <typename ...FIELDDEF>
    static SQLiteTable create (std::shared_ptr<SQLiteStorage> db, std::string name, std::tuple<FIELDDEF...> &&def) {
        SQLiteTable table(db, name);

        table.create(def);

        return table;
    }
};


template <typename ...FIELDTYPE>
inline TableDef<FIELDTYPE...> makeTableDef()
{
}

} // ns sqlite

#endif // SQLITETABLE_H
