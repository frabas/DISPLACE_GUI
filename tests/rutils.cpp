#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <myRutils.h>

#include <list>
#include <vector>
#include <tuple>

BOOST_AUTO_TEST_SUITE( RUtils )

using TestVector = std::tuple<int, int, std::vector<int>, std::vector<double>, std::vector<int> >;

static std::list<TestVector> TestSet {
    { std::make_tuple(20,4, std::vector<int>{12,13,14,15}, std::vector<double>{0.01, 0.2, 0.3, 0.49} , std::vector<int>{13,15,15,12,15,15,12,15,13,14,
                                                                                                                        15,15,15,15,14,15,14,13,15,14}) },
    { std::make_tuple(20,4, std::vector<int>{13,14,15,12}, std::vector<double>{0.2, 0.3, 0.49, 0.01} , std::vector<int>{13,15,15,12,15,15,12,15,13,14,
                                                                                                                        15,15,15,15,14,15,14,13,15,14})},
    { std::make_tuple(20,0, std::vector<int>{}, std::vector<double>{} , std::vector<int>{})},
    { std::make_tuple(20,1, std::vector<int>{12,13,14,15}, std::vector<double>{0.2} , std::vector<int>{})}
};

static std::vector<int> callDoSample(const TestVector &t) {
    auto &v2 = std::get<2>(t);
    int *val2 = nullptr;
    if (v2.size() > 0) {
        val2 = new int[v2.size()];
        for (size_t i = 0; i < v2.size(); ++i)
            val2[i] = v2[i];
    }

    auto &va22 = std::get<3>(t);
    double *va2 = nullptr;
    if (va22.size() > 0) {
        va2 = new double[va22.size()];
        for (size_t i = 0; i < va22.size(); ++i)
            va2[i] = va22[i];
    }

    auto v = do_sample(std::get<0>(t), std::get<1>(t), val2, va2);

    return v;
}

BOOST_AUTO_TEST_CASE( RUtils_do_Sample )
{
    // do_sample( int n, int nval, const int val[], double proba[])

    set_seed(1234, 5678);
    auto random_check = unif_rand();

    int testnum = 1;
    for (auto &tv : TestSet) {
        set_seed(1234, 5678);
        auto rnd = unif_rand();
        BOOST_CHECK_CLOSE(rnd, random_check, 0.0001);

        auto r = callDoSample(tv);

#if 0
        std::cout << "Test num: " << testnum << " : " << r.size() << " results. " << std::endl;
        int c = 1;
        for (auto s : r) {

            std::cout << s << ",";
            if ((c % 10) == 0)
                std::cout << std::endl;
            ++c;
        }
        std::cout << std::endl;
#endif

        auto &exp = std::get<4>(tv);

        BOOST_CHECK_EQUAL_COLLECTIONS(r.begin(), r.end(), exp.begin(), exp.end());
        ++testnum;
    }

//    BOOST_CHECK(s->getTimeSeries(displace::simulation::TimeSeriesManager::Fishprice, 0, 4) != 0);
}

BOOST_AUTO_TEST_SUITE_END()
