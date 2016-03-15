//
// Created by Federico Fuga on 03/03/16.
//

#ifndef DISPLACE_LINENUMBERREADER_H
#define DISPLACE_LINENUMBERREADER_H

#include <formatexception.h>

#include <map>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "prettyprint.h"

namespace displace {
    namespace formats {
        namespace helpers {

            class LineNumberReader {
            public:
                using Specifications = std::map<int, std::string>;

                LineNumberReader();

                bool importFromFile(const std::string &file, const Specifications &specifications);
                bool importFromStream(std::istream &stream, const Specifications &specifications);

                std::string get(const std::string &key, std::string value = std::string()) const;

                template <typename T>
                T getAs(const std::string &key) const {
                    try {
                        auto v = get(key);
                        if (v.empty())
                            return T();
                        return boost::lexical_cast<T>(v);
                    } catch (boost::bad_lexical_cast &x) {
                        std::stringstream ss;
                        ss << "Bad format for key '" << key << "' value '" << get(key) << "'";
                        throw FormatException(ss.str());
                    }
                }

                unsigned long numValues() const {
                    return mConfig.size();
                }

                friend std::ostream &operator << (std::ostream &strm, const LineNumberReader &rdr) {
                    return strm << rdr.mConfig;
                }

            private:
                bool set(const std::string &key, const std::string &value);

                using Container = std::map<std::string,std::string>;
                Container mConfig;
            };

        }
    }
}

#endif //DISPLACE_LINENUMBERREADER_H
