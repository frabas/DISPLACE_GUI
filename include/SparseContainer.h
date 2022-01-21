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

    template<int i, std::enable_if_t<i < sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int pos, std::tuple<Indexes...> indexes)
    {
        hash = hash | (static_cast<HashType>(std::get<i>(indexes)) << pos);
        pos = pos + sizeof(std::get<i>(indexes)) * 8;
        return hashBy<i + 1>(hash, pos, indexes);
    }

    template<int i, std::enable_if_t<i >= sizeof...(Indexes), int> = 0>
    HashType hashBy(HashType hash, unsigned int, std::tuple<Indexes...>)
    {
        return hash;
    }

public:
    HashType hashKey(Indexes ... indexes)
    {
        return hashBy<0>(0, 0, std::make_tuple(indexes...));
    }
};


#endif //DISPLACE_SPARSECONTAINER_H
