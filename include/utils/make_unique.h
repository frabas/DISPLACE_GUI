#ifndef MAKE_UNIQUE_H
#define MAKE_UNIQUE_H

#include <memory>

namespace utils {

    template<typename T, typename ... Ts>
    std::unique_ptr<T> make_unique(Ts &&... params)
    {
        return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
    };

}

#endif // MAKE_UNIQUE_H