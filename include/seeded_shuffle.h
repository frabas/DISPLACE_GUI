// seeded_shuffle: a random permutation that follows the simulation seed.
//
// SimModel::initRandom() seeds the global rand() from the simulation name, and
// every stochastic decision draws from it, so the same name gives the same run.
// std::random_shuffle (removed in C++17) drew from rand(); its replacement by
// std::shuffle with a std::random_device seeds from the operating system
// instead, and runs are no longer reproducible. This is the historical
// libstdc++ random_shuffle (same iteration, same rand() % (i + 1) index, same
// swap), so given the same seed it gives the same permutation as before.
#ifndef SEEDED_SHUFFLE_H
#define SEEDED_SHUFFLE_H

#include <algorithm>
#include <cstdlib>
#include <iterator>

template <typename RandomAccessIterator>
inline void seeded_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first == last)
        return;
    typedef typename std::iterator_traits<RandomAccessIterator>::difference_type diff_t;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
        RandomAccessIterator j = first + static_cast<diff_t>(std::rand() % ((i - first) + 1));
        if (i != j)
            std::iter_swap(i, j);
    }
}

#endif // SEEDED_SHUFFLE_H
