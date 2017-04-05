#ifndef PATHSHOP_H
#define PATHSHOP_H

#include <idtypes.h>

#include <memory>

class PathShop
{
    using weight_t = uint16_t;

    class Private;

    Private *d;
public:
    class Data {
    private:
        types::NodeId node;
        types::NodeId previous;

        weight_t weight;

    public:
        Data();

        void setNode(types::NodeId n) { node = n; }
        types::NodeId getNode() const { return node; }

        void setPreviousNode(types::NodeId n) { previous = n; }
        types::NodeId getPreviousNode() const { return previous; }

        void setWeight(weight_t w) { weight = w; }
        double getWeight() const { return weight; }
    };

    class NodeNotFoundException : public std::exception { using exception::exception; };

    PathShop();
    virtual ~PathShop();

    Data getNode (types::NodeId id) const;

    static PathShop readFromFiles (const std::__cxx11::string &prevstream, const std::__cxx11::string &weighstream);
};

#endif // PATHSHOP_H
