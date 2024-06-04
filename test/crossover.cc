#include <gtest/gtest.h>

#include "ga.h"
#include "helper.h"

using namespace myopta;

static void AddVariables(Problem &problem, size_t count, int upper) {
    for (size_t i = 0; i < count; i++) {
        problem.Add(new Variable(upper));
    }
}

static Solution *Allocate(SolutionPool &pool, const std::vector<int> &vals) {
    Solution *sol = pool.Allocate();
    for (size_t i = 0; i < vals.size(); i++) {
        sol->values[i] = vals[i];
    }
    return sol;
}

static std::vector<int> ToInts(Solution *sol, size_t size) {
    std::vector<int> vals;
    for (size_t i = 0; i < size; i++) {
        vals.push_back(sol->values[i]);
    }
    return vals;
}

TEST(OnePointCrossoverOperator, Perform) {
    Problem problem;
    AddVariables(problem, 5, 10);

    SolutionPool pool(20, problem.size());

    Solution *sol1 = Allocate(pool, std::vector<int>({1, 2, 3, 4, 5}));
    Solution *sol2 = Allocate(pool, std::vector<int>({6, 7, 8, 9, 0}));

    DeterministicRand rand;
    rand.SetValues(std::vector<int> {3});

    CrossoverConfig config(CrossoverMethod::OnePoint);

    auto crossover = CreateCrossoverOperator(problem, config, rand);
    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({1, 2, 3, 9, 0}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({6, 7, 8, 4, 5}));

    rand.SetValues(std::vector<int> {0});
    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({6, 7, 8, 4, 5}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({1, 2, 3, 9, 0}));

    rand.SetValues(std::vector<int> {4});
    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({6, 7, 8, 4, 0}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({1, 2, 3, 9, 5}));
}

TEST(TwoPointCrossoverOperator, Perform) {
    Problem problem;
    AddVariables(problem, 5, 10);

    SolutionPool pool(20, problem.size());

    Solution *sol1 = Allocate(pool, std::vector<int>({1, 2, 3, 4, 5}));
    Solution *sol2 = Allocate(pool, std::vector<int>({6, 7, 8, 9, 0}));

    DeterministicRand rand;
    rand.SetValues(std::vector<int> {3, 1});

    CrossoverConfig config(CrossoverMethod::TwoPoint);

    auto crossover = CreateCrossoverOperator(problem, config, rand);
    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({1, 7, 8, 9, 5}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({6, 2, 3, 4, 0}));

    rand.SetValues(std::vector<int> {2, 0});

    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({6, 2, 3, 9, 5}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({1, 7, 8, 4, 0}));

    rand.SetValues(std::vector<int> {4, 4});

    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({6, 2, 3, 9, 0}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({1, 7, 8, 4, 5}));
}

TEST(UniformCrossoverOperator, Perform) {
    Problem problem;
    AddVariables(problem, 3, 5);

    SolutionPool pool(20, problem.size());

    Solution *sol1 = Allocate(pool, std::vector<int>({1, 2, 3}));
    Solution *sol2 = Allocate(pool, std::vector<int>({4, 5, 6}));

    DeterministicRand rand;
    rand.SetValues(std::vector<int> {1, 0});

    CrossoverConfig config(CrossoverMethod::Uniform);

    auto crossover = CreateCrossoverOperator(problem, config, rand);
    crossover->Perform(*sol1, *sol2);

    EXPECT_EQ(ToInts(sol1, problem.size()), std::vector<int>({4, 2, 6}));
    EXPECT_EQ(ToInts(sol2, problem.size()), std::vector<int>({1, 5, 3}));
}
