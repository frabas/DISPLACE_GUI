//
// Created by Federico Fuga on 03/03/16.
//

#ifndef DISPLACE_KEYVALUEREADER_H
#define DISPLACE_KEYVALUEREADER_H

#include <map>
#include <string>
#include <tuple>
#include <iostream>

namespace displace {
    namespace formats {
        namespace helpers {

            class KeyValueReader {
            public:
                KeyValueReader();

                bool importFromFile(const std::string &file);
                bool importFromStream(std::istream &stream);

                bool exportToFile(const std::string &file) const;
                bool exportToStream (std::ostream &os) const;

                bool set(const std::string &key, const std::string &value);
                std::string get(const std::string &key, std::string value = std::string()) const;
                bool isSet(const std::string &key) const;

                int numKeyValues() const {
                    return mConfig.size();
                }
            private:
                using Container = std::map<std::string,std::string>;
                Container mConfig;

                std::tuple<std::string, std::string> parseLine(std::string arg);

            };

        }
    }
}

#endif //DISPLACE_KEYVALUEREADER_H
