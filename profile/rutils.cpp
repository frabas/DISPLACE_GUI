#include <tuple>
#include <vector>
#include <list>
#include <iostream>

#include <myRutils.h>

using TestVector = std::tuple<int, std::vector<int>, std::vector<double>>;

static std::list<TestVector> TestSet {
    { std::make_tuple(2000, std::vector<int>{12,13,14,15}, std::vector<double>{0.01, 0.2, 0.3, 0.49} )},
    { std::make_tuple(2000, std::vector<int>{13,14,15,12}, std::vector<double>{0.2, 0.3, 0.49, 0.01} )},
    { std::make_tuple(2000, std::vector<int>{}, std::vector<double>{})},
    { std::make_tuple(2000, std::vector<int>{12}, std::vector<double>{0.2} )} ,
    { std::make_tuple(2000, std::vector<int>{12,13,14,15}, std::vector<double>{0.2,0.2,0.0, 0.1})}
};

static std::vector<int> callDoSample(const TestVector &t)
{
    auto &c = std::get<1>(t);
    auto v = do_sample(std::get<0>(t), c.size(), c, std::get<2>(t));
    return v;
}

void profile_do_sample()
{
    std::cout << "Profiling do_sample()..." << std::endl;
    // do_sample( int n, int nval, const int val[], double proba[])

    for (int i = 0; i < 2000; ++i) {
        for (auto &tv : TestSet) {
            callDoSample(tv);
        }
    }

    std::cout << "Completed.";
}
