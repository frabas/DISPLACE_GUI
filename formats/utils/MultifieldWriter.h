#ifndef MULTIFIELDWRITER_H
#define MULTIFIELDWRITER_H

#include <string>
#include <tuple>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <type_traits>

namespace displace {
    namespace formats {
        namespace helpers {
            class MultifieldWriter {

                template <std::size_t I = 0, typename ...Ts>
                inline typename std::enable_if< I == sizeof...(Ts), void>::type
                output( std::ostream &, std::tuple<Ts...> &, const char *)
                {
                };

                template <std::size_t I = 0, typename ...Ts>
                inline typename std::enable_if< I < sizeof...(Ts)-1, void>::type
                output( std::ostream &os, std::tuple<Ts...> &tuple, const char *separator)
                {
                    os << std::get<I>(tuple) << separator;
                    output<I+1, Ts...>(os, tuple, separator);
                };

                template <std::size_t I = 0, typename ...Ts>
                inline typename std::enable_if< I == sizeof...(Ts)-1, void>::type
                output( std::ostream &os, std::tuple<Ts...> &tuple, const char *separator)
                {
                    os << std::get<I>(tuple) << "\n";
                    output<I+1, Ts...>(os, tuple, separator);
                };


            public:
                MultifieldWriter() {}

                template <typename InputIterator, typename... Ts>
                bool exportToStream(std::ostream &os, const char*separator, InputIterator first, InputIterator last)
                {
                    if (!os)
                        return false;

                    while (os && first != last) {
                        output(os, *first, separator);
                        ++first;
                    }

                    return true;
                }

            };
        }
    }
}



#endif // MULTIFIELDWRITER_H
