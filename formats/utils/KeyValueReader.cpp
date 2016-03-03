//
// Created by Federico Fuga on 03/03/16.
//

#include "KeyValueReader.h"

#include <fstream>
#include <boost/algorithm/string.hpp>

#include <tuple>
#include <string>
#include <boost/regex.hpp>

using namespace displace::formats::helpers;

KeyValueReader::KeyValueReader() {

}

std::tuple<std::string,std::string> KeyValueReader::parseLine(std::string arg)
{
    boost::smatch results;

    // Legal chars for key: numbers, letters, minus, underscore, point and slash (at least one)
    // Legal chars for value: any char
    boost::regex r{ "([\\w\\-_\\.\\/]+)\\s*=\\s*(.*)"};
    if (!boost::regex_match(arg, results, r)) {
        return std::tuple<std::string, std::string>();
    }

    if (results.size() != 3) {
        return std::tuple<std::string, std::string>();
    }

    return std::make_tuple<std::string,std::string>(results[1], results[2]);
};


bool KeyValueReader::importFromFile(const std::string &file)
{
    std::ifstream f (file.c_str(), std::ios_base::in);
    return importFromStream(f);
}

bool KeyValueReader::importFromStream(std::istream &is)
{
    if (!is)
        return false;

    while (is) {
        std::string line;
        std::getline(is, line);

        //std::cout << "Line: " << line << std::endl;

        boost::trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        if (!is)
            break;

        auto kv = parseLine(line);
        auto key = std::get<0>(kv);
        auto value = std::get<1>(kv);
        boost::trim(value);

        //std::cout << "parse: " << key << "|" << value << std::endl;

        if (key.empty())
            return false;

        set(key, value);
    }

    return true;
}

bool KeyValueReader::exportToFile(const std::string &file) const
{
    std::ofstream f (file.c_str(), std::ios_base::out);
    return exportToStream(f);
}

bool KeyValueReader::exportToStream(std::ostream &os) const
{
    if (!os)
        return false;

    for (auto kv : mConfig) {
        os << kv.first << " = " << kv.second << std::endl;
    }

    return true;
}

bool KeyValueReader::set(const std::string &key, const std::string &value)
{
    bool found;

    auto r = mConfig.find(key);
    if (r == mConfig.end()) {
        // not found, add it
        found = false;
        mConfig.insert(std::make_pair(key,value));
    } else {
        // found, subst
        found = true;
        r->second = value;
    }

    return found;
}

std::string KeyValueReader::get(const std::string &key, std::string value) const
{
    auto r = mConfig.find(key);
    if (r == mConfig.end()) {
        return value;
    }
    return r->second;
}

bool KeyValueReader::isSet(const std::string &key) const
{
    auto r = mConfig.find(key);
    return r != mConfig.end();
}
