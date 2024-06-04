#include "ga.h"

#include <algorithm>

namespace myopta {

GeneticAlgorithm::GeneticAlgorithm(const Problem& problem, EvaluatorFactory& factory,
                                   const GeneticAlgorithmConfig& config, Rand& rand)
    : problem_(problem),
      rand_(rand),
      config_(config),
      pool_(config.population_size * 2, problem.size()),
      elite_set_(config.elite_count),
      evaluator_(factory, config.thread_count)  {
    for (size_t i = 0; i < 2; i++) {
        populations_[i].reserve(config.population_size);
    }
    crossover_ = CreateCrossoverOperator(problem, config_.crossover, rand_);
}

void GeneticAlgorithm::InitPopulation(Population& population, Rand& rand) {
    for (size_t i = 0; i < config_.population_size; i++) {
        auto solution = pool_.Allocate();
        InitSolution(problem_, *solution, rand);
        population.push_back(solution);
    }
}

void GeneticAlgorithm::ClearPopulation(Population& population) {
    for (auto solution : population) {
        if (!solution->elite) {
            pool_.Deallocate(solution);
        }
    }
    population.clear();
}

void GeneticAlgorithm::EvaluatePopulation(Population& population) {
    evaluator_.Evaluate(population);
    for (size_t i = 0; i < population.size(); i++) {
        auto solution = population[i];
        if (!solution->elite) {
            elite_set_.Add(solution);
        } 
    }
}

Solution* SelectByTournament(Population& population, size_t k, Rand& rand) {
    Solution* best = nullptr;
    for (size_t i = 0; i < k; i++) {
        auto solution = population[1 /*rand.next( population.size())*/];
        if (best == nullptr || best->fitness < solution->fitness) {
            best = solution;
        }
    }
    return best;
}

void GeneticAlgorithm::Mutate(Solution& sol) {
    for (size_t j = 0; j < problem_.size(); j++) {
        auto rd = rand_.next_double();
        if (rd <= config_.mutation_rate) {
            problem_.variables()[j]->Pick(sol.values[j], rand_);
        }
    }
}
bool GeneticAlgorithm::ShouldStop() {
    return iteration_count_ >= config_.max_iteration;
}

void GeneticAlgorithm::Run() {
    Population* parents = &populations_[0];
    Population* offspring = &populations_[1];

    InitPopulation(*parents, rand_);

    for (iteration_count_ = 0; !ShouldStop(); iteration_count_++) {
        ClearPopulation(*offspring);
        EvaluatePopulation(*parents);
        for (auto solution : elite_set_.data()) {
            offspring->push_back(solution);
        }
        while (offspring->size() < config_.population_size) {
            auto p1 = SelectByTournament(*parents, config_.tournament_size, rand_);
            auto p2 = SelectByTournament(*parents, config_.tournament_size, rand_);
            auto o1 = pool_.Copy(p1);
            auto o2 = pool_.Copy(p2);
            o1->elite = false;
            o2->elite = false;

            crossover_->Perform(*o1, *o2);

            Mutate(*o1);
            offspring->push_back(o1);

            if (offspring->size() < config_.population_size) {
                Mutate(*o2);
                offspring->push_back(o2);
            } else {
                pool_.Deallocate(o2);
            }
        }

        std::swap(parents, offspring);
    }
}

}  // namespace myopta