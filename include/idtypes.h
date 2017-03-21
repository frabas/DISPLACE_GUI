#ifndef IDTYPES_H
#define IDTYPES_H

#include <iostream>
#include <cstdint>

namespace types {

template <typename C, typename Type>
class Id {
private:
    C id;
public:
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


struct NodeTag {};

using NodeId = Id<uint16_t, NodeTag>;

namespace special {
    static const NodeId InvalidNodeId = NodeId((uint16_t)-1);
}

}

#endif // IDTYPES_H
