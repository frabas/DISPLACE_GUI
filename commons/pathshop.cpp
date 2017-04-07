#include "pathshop.h"

#include <legacy/binarygraphfilereader.h>

#include <sparsepp/spp.h>


namespace std
{
    // inject specialization of std::hash for Person into namespace std
    // ----------------------------------------------------------------
    template<>
    struct hash<types::NodeId>
    {
        std::size_t operator()(types::NodeId const &n) const
        {
            spp::spp_hash<types::NodeId::type>hasher;
            return hasher(n.toIndex());
        }
    };
}

class PathShop::Private {
    spp::sparse_hash_map<types::NodeId, PathShop::Data> map;
public:
    Private()
        : map() {

    }

    void set(types::NodeId node, types::NodeId previous, weight_t weight) {
        auto it = map.find(node);
        if (it == map.end()) {
            PathShop::Data data;
            data.setNode(node);
            data.setPreviousNode(previous);
            data.setWeight(weight);
            map.insert(std::make_pair(node, data));
        } else {
            it->second.setPreviousNode(previous);
            it->second.setWeight(weight);
        }
    }

    void setPrevious(types::NodeId node, types::NodeId previous) {
        auto it = map.find(node);
        if (it == map.end()) {
            PathShop::Data data;
            data.setNode(node);
            data.setPreviousNode(previous);
            map.insert(std::make_pair(node, data));
        } else {
            it->second.setPreviousNode(previous);
        }
    }

    void setWeight(types::NodeId node, weight_t w) {
        auto it = map.find(node);
        if (it == map.end()) {
            PathShop::Data data;
            data.setNode(node);
            data.setWeight(w);
            map.insert(std::make_pair(node, data));
        } else {
            it->second.setWeight(w);
        }
    }

    PathShop::Data getNode(types::NodeId id) const {
        auto it = map.find(id);
        if (it == map.end())
            throw PathShop::NodeNotFoundException();
        return it->second;
    }

};

PathShop::PathShop()
    : d(std::make_shared<Private>())
{
}

PathShop::Data PathShop::getNode(types::NodeId id) const
{
    return d->getNode(id);
}

PathShop* PathShop::readFromFiles(const std::string &prevstream, const std::string &weighstream)
{
    PathShop shop;

    displace::formats::legacy::BinaryGraphFileReader rdr;
    rdr.importFromStream<uint16_t,uint16_t>(prevstream, [&shop](uint16_t key, uint16_t value) {
        shop.d->setPrevious (types::NodeId(key), types::NodeId(value));
        return true;
    });
    displace::formats::legacy::BinaryGraphFileReader wrdr;
    wrdr.importFromStream<uint16_t,uint16_t>(weighstream, [&shop](uint16_t key, uint16_t value) {
        shop.d->setWeight(types::NodeId(key), value);
        return true;
    });

    return &shop;
}

PathShop PathShop::readFromData(const std::vector<InitData> &data)
{
    PathShop shop;

    for (auto d : data) {
        shop.d->set(types::NodeId(std::get<0>(d)), types::NodeId(std::get<1>(d)), weight_t(std::get<2>(d)));
    }
    return shop;
}

