#ifndef VECTORSDATA_H
#define VECTORSDATA_H

#include <vector>

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
        ResultType v = boost::lexical_cast<ResultType>(x);
        c.push_back(v);
    }

    return c;
}

}
}
}


#endif // VECTORSDATA_H
