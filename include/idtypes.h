#ifndef IDTYPES_H
#define IDTYPES_H

#include <iostream>

#if !defined(_HAS_STD_BYTE)
#define _HAS_STD_BYTE 0
#endif
#include <cstdint>
#undef _HAS_STD_BYTE

#include <boost/lexical_cast.hpp>

namespace types {

template <typename C, typename Type>
class Id {
private:
    C id;
public:
    using type = C;
    using traits = Type;

    explicit Id(C x) : id(x) {}

    friend std::ostream &operator << (std::ostream &ss, const Id &c) {
        ss << c.id;
        return ss;
    }

    C toIndex() const { return id; }

    friend bool operator == (Id<C,Type> id1, Id<C,Type> id2) {
        return id1.id == id2.id;
    }

    friend bool operator < (Id<C,Type> id1, Id<C,Type> id2) {
        return id1.id < id2.id;
    }

    friend bool operator != (Id<C,Type> id1, Id<C,Type> id2) {
        return id1.id != id2.id;
    }

};

#if !defined (__APPLE__)
template <typename X>
bool inline isIdInvalid(const X &) = delete;
#else
template <typename X>
bool inline isIdInvalid(const X &) { return true; }
#endif

struct NodeTag {};

class NodeId : public Id<uint16_t, NodeTag> {
public:
    NodeId() : Id((uint16_t)-1) {}
    explicit NodeId(uint16_t v) : Id(v) {}
};

namespace special {
    static const NodeId InvalidNodeId;
}

template <>
bool inline isIdInvalid(const int &n) {
    return n == -1;
}

template <>
bool inline isIdInvalid(const NodeId &t) {
    return t == special::InvalidNodeId;
}

} // ns types

namespace boost {
    template<>
    inline types::NodeId lexical_cast(const std::string& arg) { return types::NodeId(boost::lexical_cast<uint16_t>(arg)); }
}

#endif // IDTYPES_H
