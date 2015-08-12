#define BOOST_TEST_DYN_LINK

#include <dtree/decisiontree.h>
#include <dtree/decisiontreemanager.h>
#include <boost/test/unit_test.hpp>

struct hello_world_fixture
{
    std::ostringstream dest;
};

BOOST_FIXTURE_TEST_SUITE(test_hello_world, hello_world_fixture);

BOOST_AUTO_TEST_CASE( Dtree )
{
    dtree::DecisionTreeManager *manager = dtree::DecisionTreeManager::manager();

    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) != false );
}


BOOST_AUTO_TEST_SUITE_END();
