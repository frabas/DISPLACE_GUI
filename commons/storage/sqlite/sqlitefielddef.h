#ifndef SQLITEFIELDDEF_H
#define SQLITEFIELDDEF_H

namespace sqlite {

namespace FieldType {
struct Null {};
struct Integer {};
struct Real{};
struct Text{};
struct Blob{};
}

enum FieldAttribute : int{
    PrimaryKey = 0x01,
    NotNull = 0x02,
    Unique = 0x04,
    AutoIncrement = 0x08
};

template <typename FIELDTYPE>
class FieldDef {
private:
    std::string fieldName;
    int attributes = 0;

public:
    explicit FieldDef (std::string name)
        : fieldName(std::move(name)) {}

    std::string name() const { return fieldName; }
    inline std::string sqlType() const {
        static_assert(sizeof(FIELDTYPE) == 0, "Generic version of sqlType is undefined");
    }

    inline std::string sqlAttributes() const {
        std::ostringstream ss;

        if (attributes & FieldAttribute::PrimaryKey)
            ss << " PRIMARY KEY";
        if (attributes & FieldAttribute::AutoIncrement)
            ss << " AUTOINCREMENT";
        if (attributes & FieldAttribute::NotNull)
            ss << " NOT NULL";
        if (attributes & FieldAttribute::Unique)
            ss << " UNIQUE";

        return ss.str();
    }

    FieldDef<FIELDTYPE> &primaryKey() {
        attributes |= FieldAttribute::PrimaryKey;
        return *this;
    }
    FieldDef<FIELDTYPE> &notNull() {
        attributes |= FieldAttribute::NotNull;
        return *this;
    }
    FieldDef<FIELDTYPE> &unique() {
        attributes |= FieldAttribute::Unique;
        return *this;
    }
    FieldDef<FIELDTYPE> &autoincrement() {
        attributes |= FieldAttribute::AutoIncrement;
        return *this;
    }
};

template <>
inline std::string FieldDef<FieldType::Integer>::sqlType() const { return "INTEGER"; }

template <>
inline std::string FieldDef<FieldType::Text>::sqlType() const { return "TEXT"; }

template <>
inline std::string FieldDef<FieldType::Real>::sqlType() const { return "REAL"; }


template <typename FIELDTYPE>
inline FieldDef<FIELDTYPE> makeFieldDef(std::string name, FIELDTYPE type) {
    return FieldDef<FIELDTYPE>(name);
}

} // ns sqlite


#endif // SQLITEFIELDDEF_H
