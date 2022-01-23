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
    std::vector<size_t> maxDim;

    Value defaultValue = {};

    template<int i, typename ...GenIndexes, std::enable_if_t<i < sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int pos, std::tuple<GenIndexes...> indexes) const
    {
        auto index = static_cast<HashType>(std::get<i>(indexes));
        hash = hash | (index << pos);
        pos = pos + sizeof(std::tuple_element_t<i, std::tuple<Indexes...>>) * 8;
        return hashBy<i + 1>(hash, pos, indexes);
    }

    template<int i, typename ...GenIndexes, std::enable_if_t<i < sizeof...(Indexes), int> = 0>
    HashType hashByRedim(HashType hash, unsigned int pos, std::tuple<GenIndexes...> indexes)
    {
        auto index = static_cast<HashType>(std::get<i>(indexes));
        maxDim[i] = std::max(maxDim[i], index + 1);
        hash = hash | (index << pos);
        pos = pos + sizeof(std::tuple_element_t<i, std::tuple<Indexes...>>) * 8;
        return hashByRedim<i + 1>(hash, pos, indexes);
    }

    template<int i, typename ...GenIndexes, std::enable_if_t<i >= sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int, std::tuple<GenIndexes...>) const
    {
        return hash;
    }

    template<int i, typename ...GenIndexes, std::enable_if_t<i >= sizeof...(Indexes), int> = 0>
    HashType hashByRedim(HashType hash, unsigned int, std::tuple<GenIndexes...>) const
    {
        return hash;
    }

    template<int i, typename ... Sizes, std::enable_if_t<i < sizeof...(Indexes), int> = 0>
    void setSizes(std::tuple<Sizes...> sizes)
    {
        maxDim[i] = std::get<i>(sizes);
        setSizes<i + 1>(sizes);
    };

    template<int i, typename ... Sizes, std::enable_if_t<i >= sizeof...(Indexes), int> = 0>
    void setSizes(std::tuple<Sizes...> sizes)
    {
    };

    template<typename ... GenIndexes>
    HashType hashKeyRedim(GenIndexes ... indexes)
    {
        return hashByRedim<0>(0, 0, std::make_tuple(indexes...));
    }

public:
    SparseContainer() : maxDim(sizeof...(Indexes))
    {
    }

    template<typename ... Sizes>
    SparseContainer(Sizes ...sizes) : maxDim(sizeof...(Indexes))
    {
        setSizes<0>(std::make_tuple(sizes...));
    }

    template<typename ... GenIndexes>
    HashType hashKey(GenIndexes ... indexes) const
    {
        return hashBy<0>(0, 0, std::make_tuple(indexes...));
    }

    template<typename ... GenIndexes>
    Value &operator()(GenIndexes ... indexes)
    {
        return hashMap[hashKeyRedim(indexes...)];
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
    bool hasValue(GenIndexes ... indexes) const
    {
        auto k = hashKey(indexes...);
        return (hashMap.contains(k));
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

    size_t allocated_size() const
    {
        return hashMap.size();
    }

    size_t dimension(int index) const
    {
        return maxDim[index];
    }
};


#endif //DISPLACE_SPARSECONTAINER_H
