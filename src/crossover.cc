#include "crossover.h"

#include <assert.h>

#include <algorithm>

namespace myopta {

inline int Round(double value) {
    return int(value + .5);
}

class SinglePointCrossoverOperator : public CrossoverOperator {
  private:
    Rand& rand_;
    size_t size_;

  public:
    SinglePointCrossoverOperator(Rand& rand, size_t size) : rand_(rand), size_(size) {}

    void Perform(Solution& sol1, Solution& sol2) override {
        int crossover_point = rand_.next(size_);
        for (int i = crossover_point; i < size_; ++i) {
            std::swap(sol1.values[i], sol2.values[i]);
        }
    }
};

class TwoPointCrossoverOperator : public CrossoverOperator {
  private:
    Rand& rand_;
    size_t size_;

  public:
    TwoPointCrossoverOperator(Rand& rand, size_t size) : rand_(rand), size_(size) {}

    void Perform(Solution& sol1, Solution& sol2) override {
        int point1 = rand_.next(size_);
        int point2 = rand_.next(size_);
        if (point1 > point2) {
            std::swap(point1, point2);
        }

        for (int i = point1; i <= point2; ++i) {
            std::swap(sol1.values[i], sol2.values[i]);
        }
    }
};
class UniformCrossoverOperator : public CrossoverOperator {
  private:
    Rand& rand_;
    size_t size_;

  public:
    UniformCrossoverOperator(Rand& rand, size_t size) : rand_(rand), size_(size) {}

    void Perform(Solution& sol1, Solution& sol2) override {
        for (int i = 0; i < size_; ++i) {
            if (rand_.next(2)) {
                std::swap(sol1.values[i], sol2.values[i]);
            }
        }
    }
};

std::unique_ptr<CrossoverOperator> CreateCrossoverOperator(const Problem& problem, const CrossoverConfig& config,
        Rand& rand) {
    switch (config.method) {
    case CrossoverMethod::OnePoint:
        return std::make_unique<SinglePointCrossoverOperator>(rand, problem.size());
    case CrossoverMethod::TwoPoint:
        return std::make_unique<TwoPointCrossoverOperator>(rand, problem.size());
    case CrossoverMethod::Uniform:
        return std::make_unique<UniformCrossoverOperator>(rand, problem.size());
    default:
        assert(0);
    }
}

}  // namespace myopta