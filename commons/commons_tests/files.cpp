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
    double find_ii (const std::multimap<int,int> &map, int key) {
        return find<int,int>(map, key);
    }
    std::string find_is (const std::multimap<int,std::string> &map, int key) {
        return find<int,std::string>(map, key);
    }

    template <typename KEY, typename VALUE>
    void print_mmap(ostream &out, const std::multimap<KEY, VALUE> &mmap) {
        for (auto itr : mmap) {
            out << itr.first << "," <<itr.second << "\n";
        }
    }
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_id )
{
    std::string testsuite1 = "headings\n0 1.1\n1 1.200\r\n2 0\n3 100.0    \n4 4\n\r";

    std::istringstream is(testsuite1);
    std::multimap<int,double> info;

    fill_multimap_from_specifications_i_d(is, info);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_CLOSE(find_id(info,0), 1.1, 1);
    BOOST_CHECK_CLOSE(find_id(info,1), 1.2, 1);
    BOOST_CHECK_CLOSE(find_id(info,2), 0, 1);
    BOOST_CHECK_CLOSE(find_id(info,3), 100, 1);
    BOOST_CHECK_CLOSE(find_id(info,4), 4, 1);
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_ii )
{
    std::string testsuite1 = "headings\n0 0\n1 1\r\n2 2\n3 3    \n4 4\n\rskipme\n";

    std::istringstream is(testsuite1);
    std::multimap<int,int> info;

    fill_multimap_from_specifications_i_i(is, info);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_EQUAL(find_ii(info,0), 0);
    BOOST_CHECK_EQUAL(find_ii(info,1), 1);
    BOOST_CHECK_EQUAL(find_ii(info,2), 2);
    BOOST_CHECK_EQUAL(find_ii(info,3), 3);
    BOOST_CHECK_EQUAL(find_ii(info,4), 4);
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_is )
{
    std::string testsuite1 = "headings\n0 a\n1 b\r\n2 c\n3 d    \n4 e\n\rskipme\n";

    std::istringstream is(testsuite1);
    std::multimap<int,std::string> info;

    fill_multimap_from_specifications_i_s(is, info);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_EQUAL(find_is(info,0), std::string{"a"});
    BOOST_CHECK_EQUAL(find_is(info,1), std::string{"b"});
    BOOST_CHECK_EQUAL(find_is(info,2), std::string{"c"});
    BOOST_CHECK_EQUAL(find_is(info,3), std::string{"d"});
    BOOST_CHECK_EQUAL(find_is(info,4), std::string{"e"});
}
