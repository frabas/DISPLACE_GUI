//
// Created by Federico Fuga on 03/03/16.
//

#ifndef DISPLACE_LINENUMBERREADER_H
#define DISPLACE_LINENUMBERREADER_H

#include <map>
#include <string>

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
