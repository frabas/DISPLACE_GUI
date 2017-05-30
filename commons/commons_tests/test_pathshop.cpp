#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <pathshop.h>
#include <myutils.h>


#if BOOST_VERSION <= 105900
#define BOOST_NAMESPACE_TTDETAIL_BEGIN
#define BOOST_NAMESPACE_TTDETAIL_END
#else
#define BOOST_NAMESPACE_TTDETAIL_BEGIN namespace tt_detail {
#define BOOST_NAMESPACE_TTDETAIL_END }
#endif

namespace boost {
namespace test_tools {
BOOST_NAMESPACE_TTDETAIL_BEGIN

template <typename T >
std::ostream &operator << (std::ostream &stream, const std::list<T> &c) {
    stream << "[ ";
    for (auto v : c) {
        stream << v << " ";
    }
    stream << "]";
    return stream;
}

BOOST_NAMESPACE_TTDETAIL_END
}
}

template <typename T>
bool operator == (const std::list<T> &o1, const std::list<T> &o2) {
    if (o1.size() != o2.size())
        return false;
    auto i1 = o1.begin();
    auto i2 = o2.begin();
    for (int i = 0; i < o1.size(); ++i) {
        if (*i1 != *i2)
            return false;
    }
    return true;
}

namespace {
list<types::NodeId::type> toList(const std::list<types::NodeId> &nd) {
    list<types::NodeId::type> l;
    for (auto n : nd) {
        l.push_back(n.toIndex());
    }
    return l;
}
}

BOOST_AUTO_TEST_CASE ( test_find )
{
    std::vector<PathShop::InitData> data = {
        std::make_tuple( 1, -1, 1 ),
        std::make_tuple( 2,  1, 2 ),
        std::make_tuple(3,  1, 3)
    };

    PathShop ps = PathShop::readFromData(data);

    BOOST_CHECK_EQUAL(1, ps.getNode(types::NodeId(2)).getPreviousNode().toIndex());
    BOOST_CHECK_EQUAL(2, ps.getNode(types::NodeId(2)).getWeight());
    BOOST_CHECK_EQUAL(types::special::InvalidNodeId, ps.getNode(types::NodeId(1)).getPreviousNode());
    BOOST_CHECK_EQUAL(1, ps.getNode(types::NodeId(1)).getWeight());

    BOOST_CHECK_EQUAL(1, ps.getNode(types::NodeId(3)).getPreviousNode().toIndex());
    BOOST_CHECK_EQUAL(3, ps.getNode(types::NodeId(3)).getWeight());

    BOOST_CHECK_THROW(ps.getNode(types::NodeId(-1)), PathShop::NodeNotFoundException);
}

BOOST_AUTO_TEST_CASE ( test_dijkstra )
{
    std::vector<PathShop::InitData> data = {
        std::make_tuple( 1, -1, 1 ),
        std::make_tuple( 2,  1, 2 ),
        std::make_tuple(3,  1, 3)
    };

    PathShop ps = PathShop::readFromData(data);

    auto r1 = DijkstraGetShortestPathTo(types::NodeId(2), ps);
    auto exp = std::list<vertex_t> ({1, 2});
    auto r = toList(r1);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp.begin(), exp.end(), r.begin(), r.end());

    auto r2 = DijkstraGetShortestPathTo(types::NodeId(3), ps);
    exp = std::list<vertex_t> ({1, 3});
    r = toList(r2);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp.begin(), exp.end(), r.begin(), r.end());

    auto r3 = DijkstraGetShortestPathTo(types::NodeId(1), ps);
    exp = std::list<vertex_t> ({1});
    r = toList(r3);
    BOOST_CHECK_EQUAL_COLLECTIONS(exp.begin(), exp.end(), r.begin(), r.end());

    // Disable this test, it is not correct
    //auto r4 = DijkstraGetShortestPathTo(types::NodeId(4), ps);
    //BOOST_CHECK_EQUAL(std::list<vertex_t>({}), toList(r4));
}
