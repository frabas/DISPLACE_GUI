#ifndef PATHSHOP_H
#define PATHSHOP_H

#include <commons_global.h>
#include <idtypes.h>

#include <memory>
#include <string>
#include <vector>
#include <tuple>

class  PathShop
{
    using weight_t = uint16_t;

    class Private;

    std::shared_ptr<Private> d;
public:
    class Data {
    private:
        types::NodeId node;
        types::NodeId previous;

        weight_t weight = 0;

    public:
        Data() {}

        void setNode(types::NodeId n) { node = n; }
        types::NodeId getNode() const { return node; }

        void setPreviousNode(types::NodeId n) { previous = n; }
        types::NodeId getPreviousNode() const { return previous; }

        void setWeight(weight_t w) { weight = w; }
        double getWeight() const { return weight; }
    };

    class NodeNotFoundException : public std::exception { using exception::exception; };

    PathShop();
    virtual ~PathShop() noexcept = default;

    Data getNode (types::NodeId id) const;

    static PathShop readFromFiles (const std::string &prevstream, const std::string &weighstream);

    using InitData = std::tuple<types::NodeId::type, types::NodeId::type, weight_t>;
    static PathShop readFromData(const std::vector<InitData> &data);
};

#endif // PATHSHOP_H
