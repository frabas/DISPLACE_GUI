//
// Created by Federico Fuga on 03/03/16.
//

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <utils/LineNumberReader.h>

#include <tuple>

using namespace displace::formats::helpers;

using Result = std::list<std::pair<std::string, std::string>>;

using LineNumberTestCase = std::tuple<std::string, LineNumberReader::Specifications, Result >;

static std::list<LineNumberTestCase> testCase {
        LineNumberTestCase {
                "Line0\nEmpty\nLine2\nEmpty\nEmpty\nLine5",
                {{0, "Line0"}, {2, "Line2"}, {5, "Line5"}},
                {{"Line0", "Line0"}, {"Line2", "Line2"}, {"Line5", "Line5"}}
        }
};

BOOST_AUTO_TEST_CASE( linenumber_import )
{
    for (auto rc : testCase) {
        auto spec = std::get<1>(rc);
        auto res = std::get<2>(rc);

        displace::formats::helpers::LineNumberReader reader;

        std::istringstream ss(std::get<0>(rc));
        bool r = reader.importFromStream(ss, spec);

        BOOST_REQUIRE(r);

        BOOST_CHECK_EQUAL(res.size(), reader.numValues());

        for (auto kv : res) {
            auto v = reader.get(kv.first);
            BOOST_CHECK_EQUAL(kv.second, v);
        }
    }

}
