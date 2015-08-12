#define BOOST_TEST_DYN_LINK

#include <dtree/decisiontree.h>
#include <dtree/decisiontreemanager.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Dtree)

BOOST_AUTO_TEST_CASE( Dtree )
{
    dtree::DecisionTreeManager *manager = dtree::DecisionTreeManager::manager();

    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) == false );
}


BOOST_AUTO_TEST_SUITE_END();
