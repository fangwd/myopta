#ifndef MYOPTA_GA_H_
#define MYOPTA_GA_H_

#include <thread>
#include <condition_variable>

#include "myopta.h"
#include "crossover.h"
#include "misc.h"

namespace myopta {

struct GeneticAlgorithmConfig {
    size_t population_size;
    size_t tournament_size;
    size_t elite_count;
    size_t thread_count;
    size_t max_iteration;

    CrossoverConfig crossover;
    double mutation_rate;
};

class GeneticAlgorithm {
  private:
    const Problem& problem_;
    Rand& rand_;
    const GeneticAlgorithmConfig& config_;

    SolutionPool pool_;

    Population populations_[2];
    EliteSet elite_set_;
    ParallelEvaluator evaluator_;

    std::unique_ptr<CrossoverOperator> crossover_;

    void InitPopulation(Population&, Rand&);
    void ClearPopulation(Population&);
    void EvaluatePopulation(Population&);
    void Mutate(Solution&);

    size_t iteration_count_;
    bool ShouldStop();

    std::vector<std::thread> threads_;
    std::condition_variable cv_;

  public:
    GeneticAlgorithm(const Problem&, EvaluatorFactory&, const GeneticAlgorithmConfig&, Rand&);
    void Run();

    std::vector<Solution*>& bests() {
        return elite_set_.data();
    }

    Solution* best() {
        auto& data = elite_set_.data();
        return data.size() > 0 ? data[0] : nullptr;
    }
};

}  // namespace myopta

#endif  // MYOPTA_GA_H_