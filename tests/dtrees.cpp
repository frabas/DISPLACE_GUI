#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dtree/decisiontree.h>
#include <dtree/decisiontreemanager.h>
#include <Vessel.h>
#include <Metier.h>

BOOST_AUTO_TEST_SUITE(Dtree)

BOOST_AUTO_TEST_CASE( Dtree )
{
    dtree::DecisionTreeManager *manager = dtree::DecisionTreeManager::manager();

    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) == false );

    manager->readFromDirectory("data/tests/dtree-metiers");
    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) == true );

    boost::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);

    Metier m1(9,0,std::vector<double>(),
             std::vector<double>(),
             std::vector<double>(),
             std::vector<int>(),
             0.0f,
             0.0f,
             0.0f,
             "string gear_width_model",
             std::multimap<int, double> (),
             std::vector<int>());
    Vessel v;
    v.set_metier(&m1);

    double rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 1.0, 0.01);

    Metier m2(1,0,std::vector<double>(),
             std::vector<double>(),
             std::vector<double>(),
             std::vector<int>(),
             0.0f,
             0.0f,
             0.0f,
             "string gear_width_model",
             std::multimap<int, double> (),
             std::vector<int>());

    v.set_metier(&m2);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.0, 0.01);

    manager->readFromDirectory("data/tests/dtree-metiers-mapped");
    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) == true );

    tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.1, 0.01);

    v.set_metier(&m2);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.0, 0.01);

}


BOOST_AUTO_TEST_SUITE_END();
