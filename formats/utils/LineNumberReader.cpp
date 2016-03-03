//
// Created by Federico Fuga on 03/03/16.
//

#include "LineNumberReader.h"

#include <fstream>
#include <boost/algorithm/string.hpp>

#include <tuple>
#include <string>

using namespace displace::formats::helpers;

LineNumberReader::LineNumberReader() {

}

bool LineNumberReader::importFromFile(const std::string &file, const Specifications &specifications)
{
    std::ifstream f (file.c_str(), std::ios_base::in);
    return importFromStream(f,specifications);
}

bool LineNumberReader::importFromStream(std::istream &is, const Specifications &specifications)
{
    if (!is)
        return false;

    int linenum = 0;
    while (is) {
        std::string line;
        std::getline(is, line);

        boost::trim(line);

        auto keyit = specifications.find(linenum);
        if (keyit != specifications.end()) {
            set(keyit->second, line);
        }

        ++linenum;
    }

    return true;
}

std::string LineNumberReader::get(const std::string &key, std::string value) const
{
    auto r = mConfig.find(key);
    if (r == mConfig.end()) {
        return value;
    }
    return r->second;
}

bool LineNumberReader::set(const std::string &key, const std::string &value)
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
