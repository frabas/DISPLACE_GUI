//
// Created by <fuga@studiofuga.com> on 07/03/16.
//

#ifndef DISPLACE_MULTIFIELDREADER_H
#define DISPLACE_MULTIFIELDREADER_H

#include <string>
#include <tuple>
#include <vector>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <type_traits>

namespace displace {
    namespace formats {
        namespace helpers {
            class MultifieldReader {

                template <std::size_t I = 0, typename ...Ts>
                inline typename std::enable_if< I == sizeof...(Ts), void>::type
                parse( std::tuple<Ts...> &tuple, const std::vector<std::string> &vec)
                {
                    (void)tuple;
                    (void)vec;
                };

                template <std::size_t I = 0, typename ...Ts>
                inline typename std::enable_if< I < sizeof...(Ts), void>::type
                parse( std::tuple<Ts...> &tuple, const std::vector<std::string> &vec)
                {
                    try {
                        if (!vec[I].empty())
                            std::get<I>(tuple) = boost::lexical_cast<typename std::tuple_element<I, std::tuple<Ts...> >::type >(vec[I]);
                    } catch (boost::bad_lexical_cast &x) {
                        std::ostringstream ss;
                        ss << x.what() << " fld: " << I << " (" << vec[I] << ")";
                        throw std::runtime_error(ss.str());
                    }

                    parse<I+1, Ts...>(tuple, vec);
                };


            public:
                MultifieldReader() {}

                template <typename... Ts>
                using LoaderFunctionTuple = std::tuple<Ts...>;

                template <typename T>
                using LoaderFunction = std::function<void(T)>;

                template <typename T>
                bool importFromStream(std::istream &is, const char*separator, LoaderFunction<T> loader, int curr_linenum = 0)
                {
                    if (!is)
                        return false;

                    int linenum = curr_linenum;
                    while (is) {
                        std::string line;
                        std::getline(is, line);

                        boost::trim(line);
                        if (line.empty())
                            continue;

                        std::vector<std::string> sr;
                        boost::split(sr, line, boost::is_any_of(separator));

                        T values;
                        try {
                            parse (values, sr);
                        } catch (std::exception &x) {
                            std::ostringstream ss;
                            ss << x.what() << " line: " << linenum+1;
                            throw std::runtime_error(ss.str());
                        }

                        loader(values);

                        ++linenum;
                    }

                    return true;
                }

            };
        }
    }
}


#endif //DISPLACE_MULTIFIELDREADER_H
