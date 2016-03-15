#ifndef FORMATEXCEPTION_H
#define FORMATEXCEPTION_H

#include <stdexcept>

namespace displace {
namespace formats {

class FormatException : public std::runtime_error {
public:
    explicit FormatException(const char *msg)
        : std::runtime_error(msg) {}

    template <typename T>
    explicit FormatException(T&& mm)
        : std::runtime_error(std::forward<T>(mm)) {}
};

}
}

#endif // FORMATEXCEPTION_H
