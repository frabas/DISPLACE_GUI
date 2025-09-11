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
    { std::make_tuple(20,1, std::vector<int>{12}, std::vector<double>{0.2} , std::vector<int>{12,12,12,12,12,12,12,12,12,12,
                                                                                              12,12,12,12,12,12,12,12,12,12})} ,
    { std::make_tuple(20,4, std::vector<int>{12,13,14,15}, std::vector<double>{0.2,0.2,0.0, 0.1}, std::vector<int>{15,12,12,15,
                                                                                                                   12,12,15,12,15,
                                                                                                                   13,12,12,12,13,
                                                                                                                   13,12,13,15,12,13})}
};

static std::list<int> rand_check = {
    854157,369014,164677,998812, 92924,
    381564,991363,369306,960012, 696523,
    113424,364374,259774,480936, 518815,
    309269,515503,802267, 90733, 604089
};

static std::vector<int> callDoSample(const TestVector &t) {
    auto v = do_sample(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));

    return v;
}

BOOST_AUTO_TEST_CASE( RUtils_do_Sample )
{
    // do_sample( int n, int nval, const int val[], double proba[])

    set_seed(1234, 5678);
    double rchk;
    std::vector<int> random_check;

    rchk = unif_rand(); // drop the first value.
    for (int i = 0; i < 20; ++i) {
        random_check.push_back(static_cast<int>(std::floor(unif_rand() * 1000000 + 0.5)));
    }

    BOOST_REQUIRE_EQUAL_COLLECTIONS(random_check.begin(), random_check.end(), rand_check.begin(), rand_check.end());

    int testnum = 1;
    for (auto &tv : TestSet) {
        set_seed(1234, 5678);
        auto rnd = unif_rand();
        BOOST_CHECK_CLOSE(rnd, rchk, 0.0001);

        auto r = callDoSample(tv);

#if 0
        std::cout << "Test num: " << testnum << " : " << r.size() << " results. " << "\n";
        int c = 1;
        for (auto s : r) {

            std::cout << s << ",";
            if ((c % 10) == 0)
                std::cout << "\n";
            ++c;
        }
        std::cout << "\n";
#endif

        auto &exp = std::get<4>(tv);

        BOOST_CHECK_EQUAL_COLLECTIONS(r.begin(), r.end(), exp.begin(), exp.end());
        ++testnum;
    }
}

BOOST_AUTO_TEST_SUITE_END()
