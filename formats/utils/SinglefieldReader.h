//
// Created by Federico Fuga on 17/03/16.
//

#ifndef DISPLACE_VECTORREADER_H
#define DISPLACE_VECTORREADER_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace displace {
    namespace formats {
        namespace helpers {

            class SinglefieldReader {
            public:
                SinglefieldReader() {}

                template <typename T>
                using LoaderFunction = std::function<void(T)>;

                template <typename T>
                bool importFromStream(std::istream &is, LoaderFunction<T> loader)
                {
                    if (!is)
                        return false;

                    while (is) {
                        std::string line;
                        std::getline(is, line);

                        boost::trim(line);
                        if (line.empty())
                            continue;

                        T value = boost::lexical_cast<T>(line);
                        loader(value);
                    }

                    return true;
                }


            };

        }
    }
}

#endif //DISPLACE_VECTORREADER_H
