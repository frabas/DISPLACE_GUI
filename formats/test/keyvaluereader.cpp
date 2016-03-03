//
// Created by Federico Fuga on 03/03/16.
//

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <utils/KeyValueReader.h>

#include <tuple>

using KeyValue = std::pair<std::string,std::string>;

// 1st: List of key/values
// 2st: Input text or expected output
// 3st: use only for input
using KeyValueReaderTestCase = std::tuple<std::list<KeyValue>, std::string, bool>;

static std::list<KeyValueReaderTestCase> testCase {
    KeyValueReaderTestCase {
        {
            {"key1", "value1"},
            {"key2", "value2"}
        }, "key1 = value1\nkey2 = value2\n", false
    },
    KeyValueReaderTestCase {
        {
            {"key1empty", ""},
            {"key2", "value2"},
        }, "key1empty = \nkey2 = value2", true
    },
    KeyValueReaderTestCase {
        {
            {"key1", "value1"},
            {"key2", "value2"}
        }, "   key1 = value1\nkey2 = value2   \n\n   \n", true
    },
    KeyValueReaderTestCase {
        {
            {"key1", "value1"},
            {"key2", "value2"}
        }, "# sample Comment\nkey1 = value1\nkey2 = value2\n\n   \n", true
    },
};

BOOST_AUTO_TEST_CASE( keyvaluereader_import )
{
    for (auto rc : testCase) {
        auto kvl = std::get<0>(rc);

        displace::formats::helpers::KeyValueReader reader;

        std::istringstream ss(std::get<1>(rc));
        bool r = reader.importFromStream(ss);

        BOOST_REQUIRE(r);

        BOOST_CHECK_EQUAL(kvl.size(), reader.numKeyValues());

        for (auto kv : kvl) {
            auto v = reader.get(kv.first);
            BOOST_CHECK_EQUAL(kv.second, v);
        }
    }

}
