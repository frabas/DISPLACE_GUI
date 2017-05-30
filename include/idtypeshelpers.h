#ifndef IDTYPESHELPERS_H
#define IDTYPESHELPERS_H

#include <vector>
#include <map>

namespace types {
namespace helpers {

template <typename IDX, typename C>
std::vector<IDX> toIdVector(const std::vector<C> &in) {
    std::vector<IDX> out;
    for (auto inv : in) {
        out.emplace_back(IDX(inv));
    }
    return out;
}

// maps

template <typename KIDX, typename VIDX, typename K, typename V>
std::map<KIDX,VIDX> toIdMap (const std::map<K,V> &in) {
    std::map<KIDX,VIDX> out;
    for (auto inv : in) {
        out.insert(std::make_pair(KIDX(std::get<0>(inv)), VIDX(std::get<1>(inv))));
    }
    return out;
}


template <typename IDX, typename K, typename V>
std::map<IDX,V> toKeyIdMap (const std::map<K,V> &in) {
    std::map<IDX,V> out;
    for (auto inv : in) {
        out.insert(std::make_pair(IDX(std::get<0>(inv)), std::get<1>(inv)));
    }
    return out;
}

template <typename IDX, typename K, typename V>
std::map<K,IDX> toValueIdMap (const std::map<K,V> &in) {
    std::map<K,IDX> out;
    for (auto inv : in) {
        out.insert(std::make_pair(std::get<0>(inv), IDX(std::get<1>(inv))));
    }
    return out;
}

// multimaps

template <typename KIDX, typename VIDX, typename K, typename V>
std::multimap<KIDX,VIDX> toIdMultimap (const std::multimap<K,V> &in) {
    std::multimap<KIDX,VIDX> out;
    for (auto inv : in) {
        out.insert(std::make_pair(KIDX(std::get<0>(inv)), VIDX(std::get<1>(inv))));
    }
    return out;
}


template <typename IDX, typename K, typename V>
std::multimap<IDX,V> toKeyIdMultimap (const std::multimap<K,V> &in) {
    std::multimap<IDX,V> out;
    for (auto inv : in) {
        out.insert(std::make_pair(IDX(std::get<0>(inv)), std::get<1>(inv)));
    }
    return out;
}

template <typename IDX, typename K, typename V>
std::multimap<K,IDX> toValueIdMultimap (const std::multimap<K,V> &in) {
    std::multimap<K,IDX> out;
    for (auto inv : in) {
        out.insert(std::make_pair(std::get<0>(inv), IDX(std::get<1>(inv))));
    }
    return out;
}


}
}

#endif // IDTYPESHELPERS_H
