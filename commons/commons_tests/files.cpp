#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <map>

//BOOST_AUTO_TEST_SUITE( Files )

namespace {
    template <typename KEY, typename VALUE>
    VALUE find (const std::multimap<KEY,VALUE> &map, KEY key) {
        auto it = map.find(key);
        if (it != map.end())
            return it->second;
        std::ostringstream ss;
        ss << "Key not found: " << key;
        throw std::out_of_range(ss.str());
    }

    double find_id (const std::multimap<int,double> &map, int key) {
        return find<int,double>(map, key);
    }
}

BOOST_AUTO_TEST_CASE( files_test )
{
    std::string testsuite1 = "#headings\n0 1.1\n1 1.200\r\n2 0\n3 100.0    \n4 4  ";

    std::istringstream is(testsuite1);
    std::multimap<int,double> info;

    fill_multimap_from_specifications_i_d(is, info);

//    for (auto i : info) {
//        std::cout << i.first << "," <<i.second << "\n";
//    }

    #define CHECK(idx, value) { auto v = find<int,double>(info, 0); BOOST_CHECK_CLOSE(v, value, 1e-3); }

    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_CLOSE(find_id(info, 0), 1.1, 1);
    BOOST_CHECK_CLOSE(find_id(info,1), 1.2, 1);
    BOOST_CHECK_CLOSE(find_id(info,2), 0, 1);
    BOOST_CHECK_CLOSE(find_id(info,3), 100, 1);
    BOOST_CHECK_CLOSE(find_id(info,4), 4, 1);

    #undef CHECK
}
