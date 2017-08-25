#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dtree/decisiontree.h>
#include <dtree/decisiontreemanager.h>
#include <Vessel.h>
#include <Metier.h>

#include <dtree/vesselsevaluators.h>

BOOST_AUTO_TEST_SUITE(Dtree)

BOOST_AUTO_TEST_CASE( DTree_MonthIs )
{
    dtree::vessels::VesselMonthIsStateEvaluator eval;

    BOOST_CHECK_CLOSE(0.0, eval.evaluate(0,nullptr), 1);
    BOOST_CHECK_CLOSE(0.0909, eval.evaluate(730,nullptr), 1);
    BOOST_CHECK_CLOSE(0.5454, eval.evaluate(4350,nullptr), 1);
    BOOST_CHECK_CLOSE(0.0, eval.evaluate(8640,nullptr), 1);
}

BOOST_AUTO_TEST_CASE ( DTree_VesselEndOfTheDayIs )
{
    Vessel v1;
    VesselCalendar cal1(0,6,19,6);
    v1.updateCalendar(cal1);
    dtree::vessels::VesselEndOfTheDayIsStateEvaluator eval1;

    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(0, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(5, &v1), 1);
    BOOST_CHECK_CLOSE(0.0, eval1.evaluate(6, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(7, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(18, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(19, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(20, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(29, &v1), 1);
    BOOST_CHECK_CLOSE(0.0, eval1.evaluate(30, &v1), 1);
    BOOST_CHECK_CLOSE(1.0, eval1.evaluate(31, &v1), 1);


    Vessel v2;
    VesselCalendar cal2(0,6,6,22);
    v2.updateCalendar(cal2);
    dtree::vessels::VesselEndOfTheDayIsStateEvaluator eval2;

    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(0, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(5, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(6, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(7, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(21, &v2), 1);
    BOOST_CHECK_CLOSE(0.0, eval2.evaluate(22, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(23, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(45, &v2), 1);
    BOOST_CHECK_CLOSE(0.0, eval2.evaluate(46, &v2), 1);
    BOOST_CHECK_CLOSE(1.0, eval2.evaluate(47, &v2), 1);

}

// TODO fix this
#if 0
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
#endif

BOOST_AUTO_TEST_SUITE_END();
