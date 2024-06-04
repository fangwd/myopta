#include "misc.h"

#include <gtest/gtest.h>

#include "ga.h"

using namespace myopta;

TEST(GeneticAlgorithm, Run) {
    size_t size = 100;

    Problem problem(size);
    for (size_t i = 0; i < size; i++) {
        problem.Add(new Variable(2));
    }

    GeneticAlgorithmConfig config{.population_size = 10,
                                  .tournament_size = 2,
                                  .elite_count = 5,
                                  .thread_count = 4,
                                  .max_iteration = 10000,
    .crossover = CrossoverConfig({
        CrossoverMethod::OnePoint,
        0.6,
    }),
    .mutation_rate = 0.1};

    class MyEvaluator : public Evaluator {
      private:
        const Problem& problem_;
      public:
        MyEvaluator(const Problem& problem) : problem_(problem) {}
        void Evaluate(Solution& solution) override {
            long fitness = 0;
            for (size_t i = 0; i < problem_.size(); i++) {
                fitness += solution.values[i];
            }
            solution.fitness = fitness;
        }
    };

    class MyEvaluatorFactory : public EvaluatorFactory {
      private:
        const Problem& problem_;

      public:
        MyEvaluatorFactory(const Problem& problem) : problem_(problem) {}
        std::shared_ptr<Evaluator> CreateEvaluator() override {
            return std::make_shared<MyEvaluator>(problem_);
        }
    };

    MyEvaluatorFactory factory(problem);
    Random rand(123);

    GeneticAlgorithm ga(problem, factory, config, rand);
    ga.Run();

    EXPECT_GT(ga.best()->fitness, 0.9);
}

#define OBJ_COUNT 100
#define BIN_COUNT 100
#define BIN_SIZE  200

TEST(GeneticAlgorithm, Example) {
    Random rand(123);

    std::vector<size_t> object_sizes;
    for (size_t i = 0; i < OBJ_COUNT; i++) {
        object_sizes.push_back(rand.next(BIN_SIZE / 2));
    }

    Problem problem(OBJ_COUNT);
    for (size_t i = 0; i < OBJ_COUNT; i++) {
        problem.Add(new Variable(BIN_COUNT));
    }

    GeneticAlgorithmConfig config{.population_size = 100,
                                  .tournament_size = 20,
                                  .elite_count = 50,
                                  .thread_count = 4,
                                  .max_iteration = 1000,
                                  .crossover = CrossoverConfig({
                                      CrossoverMethod::OnePoint,
                                      0.6,
                                  }),
                                  .mutation_rate = 0.1};

    class MyEvaluator : public Evaluator {
       private:
        const Problem& problem_;
        const std::vector<size_t>& object_sizes_;

        std::vector<size_t> bin_sizes_;
        std::set<size_t> bin_indexes_;

       public:
        MyEvaluator(const Problem& problem, const std::vector<size_t>& object_sizes)
            : problem_(problem), object_sizes_(object_sizes) {
            bin_sizes_.resize(BIN_COUNT);
        }
        void Evaluate(Solution& solution) override {
            std::fill(bin_sizes_.begin(), bin_sizes_.end(), 0);
            bin_indexes_.clear();
            for (size_t i = 0; i < problem_.size(); i++) {
                size_t bin_index = solution.values[i];
                bin_sizes_[bin_index] += object_sizes_[i];
                if (bin_sizes_[bin_index] > BIN_SIZE) {
                    solution.fitness = INVALID_FITNESS;
                    return;
                }
                bin_indexes_.insert(bin_index);
            }
            solution.fitness = 1.0 / bin_indexes_.size();
        }
    };

    class MyEvaluatorFactory : public EvaluatorFactory {
       private:
        const Problem& problem_;
        const std::vector<size_t> object_sizes_;

       public:
        MyEvaluatorFactory(const Problem& problem, const std::vector<size_t>& object_sizes)
            : problem_(problem), object_sizes_(object_sizes) {}
        std::shared_ptr<Evaluator> CreateEvaluator() override {
            return std::make_shared<MyEvaluator>(problem_, object_sizes_);
        }
    };

    MyEvaluatorFactory factory(problem, object_sizes);
    GeneticAlgorithm ga(problem, factory, config, rand);
    ga.Run();

    EXPECT_LT(ga.best()->fitness, 0.5);
}
