#ifndef STATSUTILS_H
#define STATSUTILS_H

namespace stats {

template <typename T, template<typename,typename> class Container>
inline void runningSum(Container<T, std::allocator<T>> &s)
{
    T sum = 0;
    for (auto &v : s) {
        sum += v;
        v = sum;
    }
}

template <typename T, template<typename,typename> class Container>
inline void runningAvg(Container<T, std::allocator<T>> &s)
{
    size_t n = 0;
    T sum = 0;
    for (auto &v : s) {
        ++n;
        sum += v;
        v = sum / static_cast<T>(n);
    }
}

template <typename T, template<typename, typename> class Container>
inline void runningNoOp(Container<T, std::allocator<T>>& s)
{
    T val = 0;
    for (auto& v : s) {
        val = v;
        v = val;
    }
}

}

#endif // STATSUTILS_H
