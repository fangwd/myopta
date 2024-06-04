#include "misc.h"

#include <gtest/gtest.h>

#include "rand.h"

using namespace myopta;

TEST(EliteSet, Add) {
    EliteSet es(3);

    Solution sol1{.fitness=7, .elite=false};
    Solution sol2{.fitness=8, .elite=false};
    Solution sol3{.fitness=6, .elite=false};
    Solution sol4{.fitness=5, .elite=false};
    Solution sol5{.fitness=9, .elite=false};

    es.Add(&sol1);
    EXPECT_EQ(es.data(), std::vector<Solution*>({&sol1}));

    es.Add(&sol2);
    EXPECT_EQ(es.data(), std::vector<Solution*>({&sol2, &sol1}));

    es.Add(&sol3);
    EXPECT_EQ(es.data(), std::vector<Solution*>({&sol2, &sol1, &sol3}));

    es.Add(&sol4);
    EXPECT_EQ(es.data(), std::vector<Solution*>({&sol2, &sol1, &sol3}));
    EXPECT_EQ(sol3.elite, true);
    EXPECT_EQ(sol4.elite, false);

    es.Add(&sol5);
    EXPECT_EQ(es.data(), std::vector<Solution*>({&sol5, &sol2, &sol1}));
    EXPECT_EQ(sol3.elite, false);
    EXPECT_EQ(sol5.elite, true);
}

static bool Equal(const std::vector<double>& a, const std::vector<double>& b, double epsilon = 1e-5) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::fabs(a[i] - b[i]) > epsilon) return false;
    }
    return true;
}

TEST(Rand, Random) {
    std::vector<int> ints;

    Random rand(123);
    for (size_t i = 0; i < 5; i++) {
        ints.push_back(rand.next());
    }
    EXPECT_EQ(ints, std::vector<int>({-1188957731, 1018954901, -39088943, 1295249578, 1087885590}));

    std::vector<double> dbls;
    for (size_t i = 0; i < 5; i++) {
        dbls.push_back(rand.next_double());
    }
    EXPECT_TRUE(Equal(dbls, std::vector<double>({0.5741294713139589, 0.258881521312438, 0.6223695706637618,
                      0.5492985802300052, 0.1622156958567652})));

    ints.clear();
    for (size_t i = 0; i < 5; i++) {
        ints.push_back(rand.next(4096));
    }
    EXPECT_EQ(ints, std::vector<int>({2792, 3261, 1096, 2370, 993}));

    std::vector<long> longs;
    for (size_t i = 0; i < 5; i++) {
        longs.push_back(rand.next_long());
    }
    EXPECT_EQ(longs, std::vector<long>({-1694783153133139413, 2746989241534039508, -457112246358890037,
                                        1210033231312349320, 1282378635546458216}));
}
