//
// Created by happycactus on 21/01/22.
//

#ifndef DISPLACE_SPARSECONTAINER_H
#define DISPLACE_SPARSECONTAINER_H

#include <sparsepp/spp.h>

template<typename Value, typename ... Indexes>
class SparseContainer {
public:
    using HashType = size_t;

private:
    spp::sparse_hash_map<HashType, Value> hashMap;

    Value defaultValue = {};

    template<int i, typename ...GenIndexes, std::enable_if_t<i < sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int pos, std::tuple<GenIndexes...> indexes) const
    {
        hash = hash | (static_cast<HashType>(std::get<i>(indexes)) << pos);
        pos = pos + sizeof(std::tuple_element_t<i, std::tuple<Indexes...>>) * 8;
        return hashBy<i + 1>(hash, pos, indexes);
    }

    template<int i, typename ...GenIndexes, std::enable_if_t<i >= sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int, std::tuple<GenIndexes...>) const
    {
        return hash;
    }

public:
    template<typename ... GenIndexes>
    HashType hashKey(GenIndexes ... indexes) const
    {
        return hashBy<0>(0, 0, std::make_tuple(indexes...));
    }

    template<typename ... GenIndexes>
    Value &operator()(GenIndexes ... indexes)
    {
        return hashMap[hashKey(indexes...)];
    }

    template<typename ... GenIndexes>
    Value const &operator()(GenIndexes ... indexes) const
    {
        auto k = hashKey(indexes...);
        if (hashMap.contains(k)) {
            return hashMap.at(hashKey(indexes...));
        } else {
            return defaultValue;
        }
    }

    template<typename ... GenIndexes>
    Value value(GenIndexes ... indexes) const
    {
        auto k = hashKey(indexes...);
        if (hashMap.contains(k)) {
            return hashMap.at(hashKey(indexes...));
        } else {
            return defaultValue;
        }
    }

    template<typename ... GenIndexes>
    void zero(GenIndexes ... indexes)
    {
        hashMap.erase(hashKey(indexes...));
    }

    void clear()
    {
        hashMap.clear();
    }

    size_t size() const
    {
        return hashMap.size();
    }
};


#endif //DISPLACE_SPARSECONTAINER_H
