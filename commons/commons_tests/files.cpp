#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <myutils.h>
#include <map>

namespace {
    template <typename KEY, typename VALUE, typename MAP >
    class Finder {
    public:
        VALUE operator () (const MAP &map, KEY key) {
            auto it = map.find(key);
            if (it != map.end())
                return it->second;
            std::ostringstream ss;
            ss << "Key not found: " << key;
            throw std::out_of_range(ss.str());
        }
    };

    template <typename KEY, typename VALUE, typename MAP>
    void print_mmap(ostream &out, const MAP &mmap) {
        for (auto itr : mmap) {
            out << itr.first << "," <<itr.second << "\n";
        }
    }
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_id )
{
    std::string testsuite1 = "headings\n5 1.1\n1 1.200\r\n2 0\n3 100.0    \n4 4\n\r-1 -0.5\n";

    std::istringstream is(testsuite1);
    std::multimap<int,double> info;
    Finder<int, double, std::multimap<int,double> > find_id;

    bool r = fill_multimap_from_specifications_i_d(is, info);
    BOOST_CHECK(r);

    BOOST_CHECK_EQUAL(info.size(), 6);
    BOOST_CHECK_CLOSE(find_id(info,5), 1.1, 1);
    BOOST_CHECK_CLOSE(find_id(info,1), 1.2, 1);
    BOOST_CHECK_CLOSE(find_id(info,2), 0, 1);
    BOOST_CHECK_CLOSE(find_id(info,3), 100, 1);
    BOOST_CHECK_CLOSE(find_id(info,4), 4, 1);
    BOOST_CHECK_CLOSE(find_id(info,-1), -0.5, 1);
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_id_fail_bad_cast )
{
    std::string testsuite1 = "headings\n5 1y\n1x";
    std::multimap<int,double> info;

    std::istringstream is(testsuite1);
    bool r = fill_multimap_from_specifications_i_d(is, info);
    BOOST_CHECK(!r);
}

// TODO enable this test (actually crashes)
#if 0
BOOST_AUTO_TEST_CASE( test_fill_multimap_id_fail_missing_field )
{
    std::string testsuite1 = "headings\n5\n1 5\n";
    std::multimap<int,double> info;

    std::istringstream is(testsuite1);
    bool r = fill_multimap_from_specifications_i_d(is, info);
    BOOST_CHECK(!r);
}
#endif

BOOST_AUTO_TEST_CASE( test_fill_multimap_ii )
{
    std::string testsuite1 = "headings\n5 0\n1 1\r\n2 2\n3 3    \n4 4\n\n-1 -5\n";

    std::istringstream is(testsuite1);
    std::multimap<int,int> info;
    Finder<int, int, std::multimap<int,int> > find_ii;

    bool r = fill_multimap_from_specifications_i_i(is, info);
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(info.size(), 6);
    BOOST_CHECK_EQUAL(find_ii(info,5), 0);
    BOOST_CHECK_EQUAL(find_ii(info,1), 1);
    BOOST_CHECK_EQUAL(find_ii(info,2), 2);
    BOOST_CHECK_EQUAL(find_ii(info,3), 3);
    BOOST_CHECK_EQUAL(find_ii(info,4), 4);
    BOOST_CHECK_EQUAL(find_ii(info,-1), -5);
}

BOOST_AUTO_TEST_CASE( test_fill_multimap_is )
{
    std::string testsuite1 = "headings\n5 a\n1 b\r\n2 c\n3 d    \n4 e\n\r\n";

    std::istringstream is(testsuite1);
    std::multimap<int,std::string> info;
    Finder<int, std::string, std::multimap<int,std::string> > find_is;

    bool r = fill_multimap_from_specifications_i_s(is, info);
    BOOST_CHECK(r);

    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_EQUAL(find_is(info,5), std::string{"a"});
    BOOST_CHECK_EQUAL(find_is(info,1), std::string{"b"});
    BOOST_CHECK_EQUAL(find_is(info,2), std::string{"c"});
    BOOST_CHECK_EQUAL(find_is(info,3), std::string{"d"});
    BOOST_CHECK_EQUAL(find_is(info,4), std::string{"e"});
}

BOOST_AUTO_TEST_CASE( test_fill_map_id )
{
    std::string testsuite1 = "headings\n5 1.1\n1 1.200\r\n2 0\n3 -100.0    \n-4 4\n\r";

    std::istringstream is(testsuite1);
    std::map<int,double> info;
    Finder<int, double, std::map<int,double> > find_id;

    bool r = fill_map_from_specifications_i_d(is, info, "dummy folder info");
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_CLOSE(find_id(info,5), 1.1, 1);
    BOOST_CHECK_CLOSE(find_id(info,1), 1.2, 1);
    BOOST_CHECK_CLOSE(find_id(info,2), 0, 1);
    BOOST_CHECK_CLOSE(find_id(info,3), -100, 1);
    BOOST_CHECK_CLOSE(find_id(info,-4), 4, 1);

//    print_mmap<int, double, std::map<int,double>>(std::cout, info);
}

BOOST_AUTO_TEST_CASE( test_fill_map_ii )
{
    std::string testsuite1 = "headings\n5 0\n1 1\r\n2 2\n3 3    \n4 4\n\r ";

    std::istringstream is(testsuite1);
    std::map<int,int> info;
    Finder<int, int, std::map<int,int> > find_ii;

    bool r = fill_map_from_specifications_i_i(is, info, "dummy folder info");
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_EQUAL(find_ii(info,5), 0);
    BOOST_CHECK_EQUAL(find_ii(info,1), 1);
    BOOST_CHECK_EQUAL(find_ii(info,2), 2);
    BOOST_CHECK_EQUAL(find_ii(info,3), 3);
    BOOST_CHECK_EQUAL(find_ii(info,4), 4);
}

BOOST_AUTO_TEST_CASE( test_fill_map_is )
{
    std::string testsuite1 = "headings\n5 a\n1 b\r\n2 c\n3 d    \n4 e\n\r";

    std::istringstream is(testsuite1);
    std::map<int,std::string> info;
    Finder<int, std::string, std::map<int,std::string> > find_is;

    bool r = fill_map_from_specifications_i_s(is, info, "dummy folder info");
    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL(info.size(), 5);
    BOOST_CHECK_EQUAL(find_is(info,5), std::string{"a"});
    BOOST_CHECK_EQUAL(find_is(info,1), std::string{"b"});
    BOOST_CHECK_EQUAL(find_is(info,2), std::string{"c"});
    BOOST_CHECK_EQUAL(find_is(info,3), std::string{"d"});
    BOOST_CHECK_EQUAL(find_is(info,4), std::string{"e"});
}
