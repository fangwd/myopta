#include <gtest/gtest.h>

#include "myopta.h"

using namespace myopta;

struct FakeRand : public Rand {
    std::vector<int> ivals;
    std::vector<double> dvals;
    int ival_index = 0;
    int dval_index = 0;

    FakeRand(std::vector<int> ivals, std::vector<double> dvals = {}) : ivals(ivals), dvals(dvals) {}
    FakeRand(std::vector<double> dvals = {}) : dvals(dvals) {}

    int next(int n) override {
        auto value = ivals[ival_index++];
        return value % n;
    }
    double next_double() override {
        return dvals[dval_index++];
    }
};

TEST(Variable, Pick) {
    FakeRand rand(std::vector<int>({3, 4, 5}));
    Variable dv(4);

    Value val;

    dv.Pick(val, rand);
    EXPECT_EQ(val, 3);

    dv.Pick(val, rand);
    EXPECT_EQ(val, 0);

    dv.Pick(val, rand);
    EXPECT_EQ(val, 1);
}

TEST(Solution, Init) {
    FakeRand rand(std::vector<int>({0, 3, 15, 23, 41, 1}));

    auto dv1 = new Variable(3);
    auto dv2 = new Variable(2, 5);
    auto dv3 = new Variable(2, 2);

    Problem problem;

    problem.Add(dv1);
    problem.Add(dv2);
    problem.Add(dv3);

    SolutionPool pool(5, 3);
    Solution *sol = pool.Allocate();
    InitSolution(problem, *sol, rand);

    const auto& vals = sol->values;

    EXPECT_EQ(vals[0], 0);
    EXPECT_EQ(vals[1], 2);
    EXPECT_EQ(vals[2], 2);

    InitSolution(problem, *sol, rand);

    EXPECT_EQ(vals[0], 2);
    EXPECT_EQ(vals[1], 4);
    EXPECT_EQ(vals[2], 2);

    EXPECT_FLOAT_EQ(sol->fitness, INVALID_FITNESS);
}

TEST(Solution, GreaterThan) {
    SolutionPool pool(2, 0);

    Solution *sol1 = pool.Allocate();
    Solution *sol2 = pool.Allocate();

    sol1->fitness = 2;
    sol2->fitness = 1;

    EXPECT_GT(*sol1, *sol2);
}
