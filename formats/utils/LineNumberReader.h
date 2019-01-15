//
// Created by Federico Fuga on 03/03/16.
//

#ifndef DISPLACE_LINENUMBERREADER_H
#define DISPLACE_LINENUMBERREADER_H

#include <formats_globals.h>

#include <formatexception.h>

#include <map>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>

// workarfound this bug:
// Internal error c1001 d:\agent\_work\1\s\src\vctools\compiler\cxxfe\sl\p1\c\types.c', line 4563
#if defined(_MSC_VER) && !(_MSC_VER < 1915 && _MSC_VER >= 2000)
#define AVOID_MSVC_C1001_BUG
#endif

#ifndef AVOID_MSVC_C1001_BUG
#include "prettyprint.h"
#endif

namespace displace {
    namespace formats {
        namespace helpers {

            class FORMATSSHARED_EXPORT LineNumberReader {
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
                    } catch (boost::bad_lexical_cast &) {
                        std::stringstream ss;
                        ss << "Bad format for key '" << key << "' value '" << get(key) << "'";
                        throw FormatException(ss.str());
                    }
                }

                template <typename T>
                T getAs(const std::string &key, T&& defvalue) const {
                    try {
                        auto v = get(key);
                        if (v.empty())
                            return std::forward<T>(defvalue);
                        return boost::lexical_cast<T>(v);
                    } catch (boost::bad_lexical_cast &) {
                        return std::forward<T>(defvalue);
                    }
                }


                unsigned long numValues() const {
                    return mConfig.size();
                }

#ifndef AVOID_MSVC_C1001_BUG
                friend std::ostream &operator << (std::ostream &strm, const LineNumberReader &rdr) {
                    return strm << rdr.mConfig;
                }
#endif
            private:
                bool set(const std::string &key, const std::string &value);

                using Container = std::map<std::string,std::string>;
                Container mConfig;
            };

        }
    }
}

#endif //DISPLACE_LINENUMBERREADER_H
