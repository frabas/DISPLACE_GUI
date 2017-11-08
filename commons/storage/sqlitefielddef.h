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

enum class FieldAttribute : int{
    AutoIncrement = 0x01
};

template <typename FIELDTYPE>
class FieldDef {
private:
    std::string fieldName;
    int attributes;

public:
    explicit FieldDef (std::string name)
        : fieldName(std::move(name)) {}

    std::string name() const { return fieldName; }
    std::string sqlType() const { return "?"; }
};

/*
template <typename T>
constexpr std::string FieldDef<
*/
template <>
std::string FieldDef<FieldType::Integer>::sqlType() const { return "Integer"; }

template <typename FIELDTYPE>
inline FieldDef<FIELDTYPE> makeFieldDef(std::string name, FIELDTYPE type) {
    return FieldDef<FIELDTYPE>(name);
}

} // ns sqlite


#endif // SQLITEFIELDDEF_H
