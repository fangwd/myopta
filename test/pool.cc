#include <cfloat>
#include <gtest/gtest.h>

#include "myopta.h"

using namespace myopta;

TEST(SolutionPool, Allocate) {
    size_t capacity = 3;
    size_t length = 4;

    SolutionPool pool(capacity, length);

    auto sol1 = pool.Allocate();

    sol1->fitness = 1;
    sol1->values[length - 1] = 100;

    auto sol2 = pool.Allocate();

    sol2->fitness = 2;
    sol2->values[0] = 200;
    sol2->values[length - 1] = 300;

    EXPECT_EQ(sol1->values[length-1], 100);
    EXPECT_FLOAT_EQ(sol2->fitness, 2);
    EXPECT_EQ(sol2->values[0], 200);

    pool.Deallocate(sol1);
    EXPECT_FLOAT_EQ(sol1->fitness, 1);

    pool.Deallocate(sol2);
    EXPECT_FLOAT_EQ(sol2->fitness, 2);
    EXPECT_EQ(sol1->values[length - 1], 100);

    auto sol3 = pool.Allocate();
    EXPECT_FLOAT_EQ(sol3->fitness, 2);
    EXPECT_EQ(sol3, sol2);

    auto sol4 = pool.Allocate();
    EXPECT_EQ(sol4, sol1);
    EXPECT_EQ(sol4->values[length - 1], 100);

    auto sol5 = pool.Allocate();
    EXPECT_EQ(sol4->values[length - 1], 100);
    sol5->fitness = 3;
    sol5->values[0] = 400;
    sol5->values[length - 1] = 500;

    auto sol6 = pool.Allocate();
    EXPECT_EQ(sol6, nullptr);
    EXPECT_FLOAT_EQ(sol5->fitness, 3);
    EXPECT_EQ(sol5->values[length - 1], 500);
}

static bool Equal(const Solution& sol1, const Solution& sol2, size_t size) {
    if (std::abs(sol1.fitness - sol2.fitness) > FLT_EPSILON) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        if (sol1.values[i] != sol2.values[i]) {
            return false;
        }
    }
    return true;
}

TEST(SolutionPool, Copy) {
    size_t capacity = 3;
    size_t length = 4;

    SolutionPool pool(capacity, length);

    auto sol1 = pool.Allocate();

    sol1->fitness = 1;
    for (size_t i = 0; i < length; i++) {
        sol1->values[i] = i;
    }

    auto sol2 = pool.Copy(sol1);
    EXPECT_TRUE(Equal(*sol1, *sol2, length));
}
