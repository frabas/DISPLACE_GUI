#ifndef VECTORSDATA_H
#define VECTORSDATA_H

#include <formatexception.h>

#include <vector>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace displace {
namespace formats {
namespace utils {

template <typename ResultType>
std::vector<ResultType> stringToVector(std::string string, const char *separator = " ") {
    std::vector<ResultType> c;

    boost::trim(string);
    if (string.empty())
        return c;

    std::vector<std::string> sr;
    boost::split(sr, string, boost::is_any_of(separator));

    for (auto x : sr) {
        try {
            ResultType v = boost::lexical_cast<ResultType>(x);
            c.push_back(v);
        } catch (boost::bad_lexical_cast &) {
            std::stringstream ss;
            ss << "Bad format for value '" << x << "'";
            throw FormatException(ss.str());
        }
    }

    return c;
}

}
}
}


#endif // VECTORSDATA_H
