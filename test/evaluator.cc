#include <gtest/gtest.h>

#include <chrono>
#include <random>
#include <vector>

#include "myopta.h"

using namespace myopta;

TEST(ParallelEvaluator, EvaluatorWorker) {
    struct FakeEvaluator : public Evaluator {
        int fitness;
        int counter = 0;
        std::random_device rd;

        FakeEvaluator(int fitness) : fitness(fitness) {}

        void Evaluate(Solution& solution) {
            solution.fitness = fitness;
            counter++;

            std::uniform_int_distribution<> dis(1, 200);
            std::mt19937 gen(rd());
            int random_duration = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(random_duration));
        }
    };

    struct FakeFactory : public EvaluatorFactory {
        int counter = 0;
        std::vector<std::shared_ptr<FakeEvaluator>> evaluators;
        std::shared_ptr<Evaluator> CreateEvaluator() {
            auto evaluator = std::make_shared<FakeEvaluator>(++counter);
            evaluators.push_back(evaluator);
            return evaluator;
        }
    };

    FakeFactory factory;
    ParallelEvaluator evaluator(factory, 4);

    SolutionPool pool(100, 10);
    Population population;
    for (size_t i = 0; i < 20; i++) {
        auto solution = pool.Allocate();
        solution->fitness = 0;
        population.push_back(solution);
    }

    evaluator.Evaluate(population);

    size_t total = 0;
    for (auto evaluator : factory.evaluators) {
        EXPECT_GT(evaluator->counter, 0);
        total += evaluator->counter;
        evaluator->counter = 0;
    }

    EXPECT_EQ(total, 20);

    evaluator.Evaluate(population);

    for (auto evaluator : factory.evaluators) {
        EXPECT_GT(evaluator->counter, 0);
        total += evaluator->counter;
        evaluator->counter = 0;
    }

    EXPECT_EQ(total, 40);
}