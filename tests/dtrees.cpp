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
    BOOST_CHECK( manager->hasTreeVariable(dtree::DecisionTreeManager::GoFishing, dtree::vesselMetierIs) == true);
    BOOST_CHECK( manager->hasTreeVariable(dtree::DecisionTreeManager::GoFishing, dtree::vesselSizeIs) == false);

    std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);

    Metier m1(9,0,0.0,
             std::vector<std::vector<double>>(),
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
    BOOST_CHECK_CLOSE(rv, 0.09, 0.001);

    Metier m2(1,0,0.0,
             std::vector<std::vector<double>>(),
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
    BOOST_CHECK_CLOSE(rv, 0.01, 0.001);

    manager->readFromDirectory("data/tests/dtree-metiers-mapped");
    BOOST_CHECK( manager->hasTree(dtree::DecisionTreeManager::GoFishing) == true );

    tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);

    m1.set_name(0);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.0, 0.001);

    m1.set_name(4);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.0, 0.001);

    m1.set_name(5);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.1, 0.001);

    m1.set_name(9);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.1, 0.001);

    m1.set_name(10);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.2, 0.001);

    m1.set_name(29);
    v.set_metier(&m1);
    rv = v.traverseDtree(0, tree.get());
    BOOST_CHECK_CLOSE(rv, 0.4, 0.001);

}


BOOST_AUTO_TEST_SUITE_END();
